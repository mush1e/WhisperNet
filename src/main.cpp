#include <iostream>
#include <thread>
#include "did.h"
#include "connection.h"

using namespace whispernet;

// Function to append node details to DHT (dht.txt)
void register_node(const std::string& username, const std::string& did, const std::string& ip, int port) {
    std::ofstream dht_file("dht.txt", std::ios::app);
    if (!dht_file.is_open()) {
        std::cerr << "Failed to open DHT file for writing." << std::endl;
        return;
    }
    dht_file << username << " " << did << " " << ip << " " << port << "\n";
    dht_file.close();
}

int main() {
    // Print welcome message
    std::cout << R"(
        .-. . .-..-. .-..-. .----.----..----..----. .-. .-..----.---. 
        | |/ \| || {_} || |{ {__ | {}  } {_  | {}  }|  `| || {_{_   _}
        |  .'.  || { } || |.-._} } .--'| {__ | .-. \| |\  || {__ | |  
        `-'   `-'`-' `-'`-'`----'`-'   `----'`-' `-'`-' `-'`----'`-' 
    )" << std::endl;
    std::cout << "Stay hidden\n";

    // Initialize DID
    whispernet::DID nodeDID;

    // Display the public key (DID)
    std::cout << "Node DID (Public Key):" << std::endl;
    std::cout << nodeDID.getPublicKey() << std::endl;

    // Prompt for username
    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);

    // Prompt for port
    int port;
    std::cout << "Enter port to listen on: ";
    std::cin >> port;
    std::cin.ignore(); // Ignore the remaining newline character

    // Assume localhost for testing
    std::string ip_address = "127.0.0.1";

    // Register node in DHT
    register_node(username, nodeDID.getPublicKey(), ip_address, port);

    // Initialize Connection
    whispernet::Connection connection(username, nodeDID);
    if (!connection.start_server(port)) {
        std::cerr << "Failed to start server on port " << port << std::endl;
        return 1;
    }

    // Start accepting incoming connections in a separate thread
    std::thread server_thread(&whispernet::Connection::accept_connections, &connection);
    server_thread.detach();

    // Main loop for user commands
    while (true) {
        std::cout << "\nEnter command (connect <username> / send <username> <message> / exit): ";
        std::string command;
        std::getline(std::cin, command);

        if (command == "exit") {
            std::cout << "Exiting WhisperNet..." << std::endl;
            break;
        }

        // Parse the command
        if (command.find("connect ") == 0) {
            std::string target_username = command.substr(8);
            if (connection.send_connection_request(target_username)) {
                std::cout << "Connection request sent to " << target_username << std::endl;
            } else {
                std::cout << "Failed to send connection request to " << target_username << std::endl;
            }
        }
        else if (command.find("send ") == 0) {
            size_t first_space = command.find(' ');
            size_t second_space = command.find(' ', first_space + 1);
            if (second_space == std::string::npos) {
                std::cout << "Invalid send command. Usage: send <username> <message>" << std::endl;
                continue;
            }
            std::string target_username = command.substr(first_space + 1, second_space - first_space - 1);
            std::string message = command.substr(second_space + 1);
            connection.send_message(target_username, message);
        }
        else {
            std::cout << "Unknown command. Please use 'connect', 'send', or 'exit'." << std::endl;
        }
    }

    return 0;
}
