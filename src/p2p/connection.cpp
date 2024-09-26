#include "connection.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

namespace whispernet {

Connection::Connection(const std::string& username)
    : server_socket(-1), username(username) {}

Connection::~Connection() {
    close_connection();
}

void Connection::close_connection() {
    if (server_socket != -1) {
        close(server_socket);
        server_socket = -1;
    }
    // Close all connected peer sockets
    std::lock_guard<std::mutex> lock(peers_mutex);
    for (auto& peer : connected_peers) {
        close(peer.second);
    }
    connected_peers.clear();
}

bool Connection::start_server(int port) {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error creating server socket!" << std::endl;
        return false;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to any local address
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error binding server socket." << std::endl;
        return false;
    }

    if (listen(server_socket, 10) < 0) {
        std::cerr << "Error listening on port: " << port << std::endl;
        return false;
    }

    std::cout << "Server listening on port: " << port << std::endl;

    // Start accepting connections in a separate thread
    std::thread(&Connection::accept_connections, this).detach();

    return true;
}

void Connection::accept_connections() {
    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_addr_len = sizeof(client_addr);
        int new_socket = accept(server_socket, (sockaddr*)&client_addr, &client_addr_len);
        if (new_socket < 0) {
            std::cerr << "Error accepting connection!" << std::endl;
            continue;
        }

        // Handle the connection in a separate thread
        std::thread(&Connection::process_incoming_request, this, new_socket).detach();
    }
}

void Connection::process_incoming_request(int socket_fd) {
    // Receive the connection request message
    std::string message = receive_data(socket_fd);
    if (message.substr(0, 18) == "CONNECTION_REQUEST:") {
        std::string requester_username = message.substr(18);
        std::cout << "Received connection request from " << requester_username << std::endl;

        // Prompt the user to accept or deny
        std::cout << "Accept connection request from " << requester_username << "? [y/n]: ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer

        if (choice == 'y' || choice == 'Y') {
            // Send acceptance message
            if (!send_data(socket_fd, "ACCEPT")) {
                std::cerr << "Failed to send acceptance." << std::endl;
                close(socket_fd);
                return;
            }

            // Add to connected peers
            {
                std::lock_guard<std::mutex> lock(peers_mutex);
                connected_peers[requester_username] = socket_fd;
            }

            // Start a thread to handle incoming messages from this peer
            std::thread(&Connection::receive_messages, this, socket_fd, requester_username).detach();

        } else {
            // Send denial message
            send_data(socket_fd, "DENY");
            close(socket_fd);
            std::cout << "Denied connection request from " << requester_username << std::endl;
        }
    } else {
        std::cerr << "Received invalid message." << std::endl;
        close(socket_fd);
    }
}

bool Connection::send_connection_request(const std::string& target_username) {
    // Simulated user directory (for testing purposes)
    std::map<std::string, std::pair<std::string, int>> user_directory = {
        {"alice", {"127.0.0.1", 8081}},
        {"bob", {"127.0.0.1", 8082}},
        // Add more users as needed
    };

    auto it = user_directory.find(target_username);
    if (it == user_directory.end()) {
        std::cerr << "User " << target_username << " not found." << std::endl;
        return false;
    }

    std::string ip_address = it->second.first;
    int port = it->second.second;

    int temp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (temp_socket == -1) {
        std::cerr << "Error creating socket!" << std::endl;
        return false;
    }

    sockaddr_in peer_address{};
    peer_address.sin_family = AF_INET;
    peer_address.sin_port = htons(port);

    if (inet_pton(AF_INET, ip_address.c_str(), &peer_address.sin_addr) <= 0) {
        std::cerr << "Invalid peer IP address." << std::endl;
        close(temp_socket);
        return false;
    }

    if (connect(temp_socket, (sockaddr*)&peer_address, sizeof(peer_address)) < 0) {
        std::cerr << "Connection to peer failed!" << std::endl;
        close(temp_socket);
        return false;
    }

    // Send a connection request message
    std::string request_message = "CONNECTION_REQUEST:" + username;
    if (!send_data(temp_socket, request_message)) {
        std::cerr << "Failed to send connection request." << std::endl;
        close(temp_socket);
        return false;
    }

    // Wait for the response (accept/deny)
    std::string response = receive_data(temp_socket);
    if (response == "ACCEPT") {
        std::cout << target_username << " accepted your connection request." << std::endl;

        // Add to connected peers
        {
            std::lock_guard<std::mutex> lock(peers_mutex);
            connected_peers[target_username] = temp_socket;
        }

        // Start a thread to handle incoming messages from this peer
        std::thread(&Connection::receive_messages, this, temp_socket, target_username).detach();

        return true;
    } else if (response == "DENY") {
        std::cout << target_username << " denied your connection request." << std::endl;
        close(temp_socket);
        return false;
    } else {
        std::cerr << "Invalid response from peer." << std::endl;
        close(temp_socket);
        return false;
    }
}

void Connection::send_message(const std::string& target_username, const std::string& message) {
    std::lock_guard<std::mutex> lock(peers_mutex);
    auto it = connected_peers.find(target_username);
    if (it != connected_peers.end()) {
        if (!send_data(it->second, message)) {
            std::cerr << "Failed to send message to " << target_username << std::endl;
        }
    } else {
        std::cerr << "Not connected to " << target_username << std::endl;
    }
}

void Connection::receive_messages(int socket_fd, const std::string& peer_username) {
    while (true) {
        std::string message = receive_data(socket_fd);
        if (message.empty()) {
            // Connection closed
            std::cout << peer_username << " disconnected." << std::endl;
            close(socket_fd);

            // Remove from connected peers
            {
                std::lock_guard<std::mutex> lock(peers_mutex);
                connected_peers.erase(peer_username);
            }

            break;
        } else {
            std::cout << "[" << peer_username << "]: " << message << std::endl;
        }
    }
}

bool Connection::send_data(int socket_fd, const std::string& data) {
    ssize_t bytes_sent = send(socket_fd, data.c_str(), data.size(), 0);
    return bytes_sent >= 0;
}

std::string Connection::receive_data(int socket_fd) {
    char buffer[1024];
    ssize_t bytes_received = recv(socket_fd, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        return "";
    }
    return std::string(buffer, bytes_received);
}

} // namespace whispernet
