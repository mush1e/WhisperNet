#include "connection.h"
#include "whispernet.pb.h" 
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <fstream>
#include <sstream>

namespace whispernet {

    Connection::Connection(const std::string& username, const DID& did) 
        : server_socket(-1), username(username), did(did) {}

    Connection::~Connection() {
        close_connection();
        google::protobuf::ShutdownProtobufLibrary();
    }

    void Connection::close_connection() {
        if (server_socket != -1) { // Fixed the condition
            close(server_socket);
            server_socket = -1;
        }
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

        sockaddr_in server_addr {};
        server_addr.sin_family = AF_INET;               // IPv4
        server_addr.sin_addr.s_addr = INADDR_ANY;       // Bind to any available local IPs
        server_addr.sin_port = htons(port);             // Convert port from host to network byte order 

        if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Error binding server socket." << std::endl;
            close(server_socket);
            server_socket = -1;
            return false;
        }

        if (listen(server_socket, 10) < 0) {
            std::cerr << "Error listening on port: " << port << std::endl;
            close(server_socket);
            server_socket = -1;
            return false;
        }

        std::cout << "Server listening on port: " << port << std::endl;

        return true;
    }

    void Connection::accept_connections() {
        while (true) {
            sockaddr_in client_addr {};
            socklen_t client_addr_len = sizeof(client_addr);
            
            // Create a new socket to handle a connection with a new node
            int new_socket = accept(server_socket, (sockaddr*)&client_addr, &client_addr_len);

            if (new_socket < 0) {
                std::cerr << "Error accepting connection!" << std::endl;
                continue;
            }

            std::thread(&Connection::process_incoming_request, this, new_socket).detach();
        }
    }

    // Lookup DID based on username from DHT
    std::string Connection::lookup_did_in_dht(const std::string& username) {
        std::ifstream dht_file("dht.txt");
        if (!dht_file.is_open()) {
            std::cerr << "Failed to open DHT file." << std::endl;
            return "";
        }
        std::string uname, did, ip;
        int port;
        while (dht_file >> uname >> did >> ip >> port) {
            if (uname == username) {
                return did;
            }
        }
        return "";
    }

    // Get peer's IP and port based on username from DHT
    bool Connection::get_peer_info(const std::string& username, std::string& ip, int& port) {
        std::ifstream dht_file("dht.txt");
        if (!dht_file.is_open()) {
            std::cerr << "Failed to open DHT file." << std::endl;
            return false;
        }
        std::string uname, did, ip_addr;
        int port_num;
        while (dht_file >> uname >> did >> ip_addr >> port_num) {
            if (uname == username) {
                ip = ip_addr;
                port = port_num;
                return true;
            }
        }
        return false;
    }

    // Generate a random challenge string
    std::string Connection::generate_random_challenge() {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(100000, 999999);
        return std::to_string(dist(mt));
    }

    // Implement sign_challenge using OpenSSL
    std::string Connection::sign_challenge(const std::string& challenge) {
        EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
        if (!md_ctx) {
            throw std::runtime_error("Failed to create MD context.");
        }

        EVP_PKEY* private_key = nullptr;

        BIO* bio = BIO_new_mem_buf(did.getPrivateKey().c_str(), -1);
        if (!bio) {
            EVP_MD_CTX_free(md_ctx);
            throw std::runtime_error("Failed to create BIO for private key.");
        }

        private_key = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
        BIO_free(bio);
        if (!private_key) {
            EVP_MD_CTX_free(md_ctx);
            throw std::runtime_error("Failed to load private key.");
        }

        if (EVP_DigestSignInit(md_ctx, nullptr, EVP_sha256(), nullptr, private_key) <= 0) {
            EVP_PKEY_free(private_key);
            EVP_MD_CTX_free(md_ctx);
            throw std::runtime_error("Failed to initialize DigestSign.");
        }

        if (EVP_DigestSignUpdate(md_ctx, challenge.c_str(), challenge.size()) <= 0) {
            EVP_PKEY_free(private_key);
            EVP_MD_CTX_free(md_ctx);
            throw std::runtime_error("Failed to update DigestSign.");
        }

        size_t sig_len = 0;
        if (EVP_DigestSignFinal(md_ctx, nullptr, &sig_len) <= 0) {
            EVP_PKEY_free(private_key);
            EVP_MD_CTX_free(md_ctx);
            throw std::runtime_error("Failed to finalize DigestSign.");
        }

        std::vector<unsigned char> signature(sig_len);
        if (EVP_DigestSignFinal(md_ctx, signature.data(), &sig_len) <= 0) {
            EVP_PKEY_free(private_key);
            EVP_MD_CTX_free(md_ctx);
            throw std::runtime_error("Failed to obtain signature.");
        }

        EVP_PKEY_free(private_key);
        EVP_MD_CTX_free(md_ctx);

        // Convert signature to a base64 string for safe transmission
        // For simplicity, we'll use raw binary data here, but in production, consider base64 encoding
        return std::string(reinterpret_cast<const char*>(signature.data()), static_cast<std::string::size_type>(sig_len));
    }

    // Implement verify_signature using OpenSSL
    bool Connection::verify_signature(const std::string& signed_data, const std::string& original_data, const std::string& public_key_pem) {
        EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
        if (!md_ctx) {
            throw std::runtime_error("Failed to create MD context.");
        }

        EVP_PKEY* public_key = nullptr;

        BIO* bio = BIO_new_mem_buf(public_key_pem.c_str(), -1);
        if (!bio) {
            EVP_MD_CTX_free(md_ctx);
            throw std::runtime_error("Failed to create BIO for public key.");
        }

        public_key = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
        BIO_free(bio);
        if (!public_key) {
            EVP_MD_CTX_free(md_ctx);
            throw std::runtime_error("Failed to load public key.");
        }

        if (EVP_DigestVerifyInit(md_ctx, nullptr, EVP_sha256(), nullptr, public_key) <= 0) {
            EVP_PKEY_free(public_key);
            EVP_MD_CTX_free(md_ctx);
            throw std::runtime_error("Failed to initialize DigestVerify.");
        }

        if (EVP_DigestVerifyUpdate(md_ctx, original_data.c_str(), original_data.size()) <= 0) {
            EVP_PKEY_free(public_key);
            EVP_MD_CTX_free(md_ctx);
            throw std::runtime_error("Failed to update DigestVerify.");
        }

        int verify_result = EVP_DigestVerifyFinal(md_ctx, reinterpret_cast<const unsigned char*>(signed_data.c_str()), signed_data.size());

        EVP_PKEY_free(public_key);
        EVP_MD_CTX_free(md_ctx);

        return verify_result == 1; // 1 indicates success, 0 indicates failure, negative indicates error
    }

    void Connection::process_incoming_request(int socket_fd) {
        std::string msg = receive_data(socket_fd);

        // Class generated by protobuf
        WhisperMessage whisper_msg;

        if (!whisper_msg.ParseFromString(msg)) {
            std::cerr << "Failed to parse message." << std::endl;
            close(socket_fd);
            return;
        }

        if (whisper_msg.type() == MessageType::CONNECTION_REQUEST) {
            const ConnectionRequest& conn_req = whisper_msg.connection_request();
            std::string requester_username = conn_req.username();
            std::string requester_did = conn_req.did();
            std::cout << "Received connection request from " << requester_username << std::endl;

            // Lookup the DID in DHT based on username
            std::string expected_did = lookup_did_in_dht(requester_username);
            if (expected_did.empty()) {
                std::cerr << "Username " << requester_username << " not found in DHT." << std::endl;
                close(socket_fd);
                return;
            }

            if (expected_did != requester_did) {
                std::cerr << "DID mismatch for user " << requester_username << std::endl;
                close(socket_fd);
                return;
            }

            // Generate challenge and send it to the requester
            std::string challenge = generate_random_challenge();

            if (!send_data(socket_fd, challenge)) {
                std::cerr << "Failed to send challenge." << std::endl;
                close(socket_fd);
                return;
            }

            // Receive the signed challenge from the requester
            std::string signed_challenge = receive_data(socket_fd);

            // Validate signed challenge against public key to authenticate
            if (!verify_signature(signed_challenge, challenge, expected_did)) {
                std::cerr << "Failed to verify challenge response." << std::endl;
                close(socket_fd);
                return;
            }

            // Prompt user to accept the connection
            std::cout << "Accept incoming connection from " << requester_username << " [y/n]: ";
            std::string choice;
            std::getline(std::cin, choice);

            WhisperMessage response_msg;
            response_msg.set_type(MessageType::CONNECTION_RESPONSE);
            ConnectionResponse* conn_res = response_msg.mutable_connection_response();

            if (choice == "y" || choice == "Y") {
                conn_res->set_accepted(true);
                conn_res->set_message("Connection accepted.");
                std::string serialized_response;
                response_msg.SerializeToString(&serialized_response);

                send_data(socket_fd, serialized_response);

                // Add to connected peers
                {
                    std::lock_guard<std::mutex> lock(peers_mutex);
                    connected_peers[requester_username] = socket_fd;
                }

                // Start a thread to handle incoming messages
                std::thread(&Connection::receive_messages, this, socket_fd, requester_username).detach();
            } else {
                conn_res->set_accepted(false);
                conn_res->set_message("Connection denied.");
                std::string serialized_response;
                response_msg.SerializeToString(&serialized_response);

                send_data(socket_fd, serialized_response);
                close(socket_fd);
            }
        } 
        else if (whisper_msg.type() == MessageType::CONNECTION_RESPONSE) {
            // Handle connection responses if needed
            const ConnectionResponse& conn_res = whisper_msg.connection_response();
            if (conn_res.accepted()) {
                std::cout << "Connection accepted by peer: " << conn_res.message() << std::endl;
            } else {
                std::cout << "Connection denied by peer: " << conn_res.message() << std::endl;
            }
        }
        else if (whisper_msg.type() == MessageType::CHAT_MESSAGE) {
            // Handle incoming chat messages
            const ChatMessage& chat_msg = whisper_msg.chat_message();
            std::cout << "[" << chat_msg.sender() << "]: " << chat_msg.content() << std::endl;
        }
        else {
            std::cerr << "Invalid or unknown message type." << std::endl;
            close(socket_fd);
        }
    }

    bool Connection::send_connection_request(const std::string& target_username) {
        std::string target_did;
        std::string target_ip;
        int target_port;

        // Lookup target's DID
        target_did = lookup_did_in_dht(target_username);
        if (target_did.empty()) {
            std::cerr << "User " << target_username << " not found in DHT." << std::endl;
            return false;
        }

        // Lookup target's IP and port
        if (!get_peer_info(target_username, target_ip, target_port)) {
            std::cerr << "Failed to get peer info for " << target_username << std::endl;
            return false;
        }

        // Now, connect to target_ip and target_port
        int temp_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (temp_socket == -1) {
            std::cerr << "Error creating socket!" << std::endl;
            return false;
        }

        sockaddr_in peer_address{};
        peer_address.sin_family = AF_INET;
        peer_address.sin_port = htons(target_port);

        if (inet_pton(AF_INET, target_ip.c_str(), &peer_address.sin_addr) <= 0) {
            std::cerr << "Invalid peer IP address." << std::endl;
            close(temp_socket);
            return false;
        }

        if (connect(temp_socket, (sockaddr*)&peer_address, sizeof(peer_address)) < 0) {
            std::cerr << "Connection to peer failed!" << std::endl;
            close(temp_socket);
            return false;
        }

        // Prepare the connection request message
        WhisperMessage request_msg;
        request_msg.set_type(MessageType::CONNECTION_REQUEST);
        ConnectionRequest* conn_request = request_msg.mutable_connection_request();
        conn_request->set_username(username);
        conn_request->set_did(did.getPublicKey()); // Replace with your actual DID

        // Serialize the message
        std::string serialized_request;
        if (!request_msg.SerializeToString(&serialized_request)) {
            std::cerr << "Failed to serialize connection request." << std::endl;
            close(temp_socket);
            return false;
        }

        // Send the connection request message
        if (!send_data(temp_socket, serialized_request)) {
            std::cerr << "Failed to send connection request." << std::endl;
            close(temp_socket);
            return false;
        }

        // Wait for the challenge from the server
        std::string challenge = receive_data(temp_socket);
        if (challenge.empty()) {
            std::cerr << "Failed to receive challenge from peer." << std::endl;
            close(temp_socket);
            return false;
        }

        // Sign the challenge using your private key
        std::string signed_challenge = sign_challenge(challenge); // Now uses real signing

        // Send the signed challenge back to the server
        if (!send_data(temp_socket, signed_challenge)) {
            std::cerr << "Failed to send signed challenge." << std::endl;
            close(temp_socket);
            return false;
        }

        // Wait for the response
        std::string response_data = receive_data(temp_socket);
        if (response_data.empty()) {
            std::cerr << "Failed to receive response from peer." << std::endl;
            close(temp_socket);
            return false;
        }

        // Deserialize the response message
        WhisperMessage response_msg;
        if (!response_msg.ParseFromString(response_data)) {
            std::cerr << "Failed to parse response message." << std::endl;
            close(temp_socket);
            return false;
        }

        if (response_msg.type() == MessageType::CONNECTION_RESPONSE) {
            const ConnectionResponse& conn_response = response_msg.connection_response();
            if (conn_response.accepted()) {
                std::cout << target_username << " accepted your connection request." << std::endl;

                // Add to connected peers
                {
                    std::lock_guard<std::mutex> lock(peers_mutex);
                    this->connected_peers[target_username] = temp_socket;
                }

                // Start a thread to handle incoming messages from this peer
                std::thread(&Connection::receive_messages, this, temp_socket, target_username).detach();

                return true;
            } else {
                std::cout << target_username << " denied your connection request." << std::endl;
                close(temp_socket);
                return false;
            }
        } else {
            std::cerr << "Invalid response from peer." << std::endl;
            close(temp_socket);
            return false;
        }
    }

    void Connection::send_message(const std::string& target_username, const std::string& message) {
        std::lock_guard<std::mutex> lock(peers_mutex);
        auto it = connected_peers.find(target_username);
        
        if (it == connected_peers.end()) {
            std::cerr << "You don't have a connection with " << target_username << std::endl;
            return; 
        }
    
        int socket_fd = it->second;
        WhisperMessage whisper_message_cht;
        whisper_message_cht.set_type(MessageType::CHAT_MESSAGE);
        ChatMessage* cht_msg = whisper_message_cht.mutable_chat_message();
        cht_msg->set_sender(username); // Corrected to set sender as the current node's username
        cht_msg->set_content(message);

        std::string serialized_message;
        if (!whisper_message_cht.SerializeToString(&serialized_message)) {
            std::cerr << "Failed to serialize chat message." << std::endl;
            return;
        }
        // Send the message
        if (!send_data(socket_fd, serialized_message)) {
            std::cerr << "Failed to send message to " << target_username << std::endl;
        }
    }

    void Connection::receive_messages(int socket_fd, const std::string& peer_username) {
        while (true) {
            std::string data = receive_data(socket_fd);
            if (data.empty()) {
                // Connection closed
                std::cout << peer_username << " disconnected." << std::endl;
                close(socket_fd);

                // Remove from connected peers
                {
                    std::lock_guard<std::mutex> lock(peers_mutex);
                    connected_peers.erase(peer_username);
                }

                break;
            }

            // Deserialize the received message
            WhisperMessage received_msg;
            if (!received_msg.ParseFromString(data)) {
                std::cerr << "Failed to parse incoming message." << std::endl;
                continue;
            }

            if (received_msg.type() == MessageType::CHAT_MESSAGE) {
                const ChatMessage& chat_msg = received_msg.chat_message();
                std::cout << "[" << chat_msg.sender() << "]: " << chat_msg.content() << std::endl;
            } 
            else {
                std::cerr << "Received unknown message type." << std::endl;
            }
        }
    }

    bool Connection::send_data(int socket_fd, const std::string& data) {
        // Send the length of the data first
        uint32_t data_length = htonl(data.size());
        if (send(socket_fd, &data_length, sizeof(data_length), 0) != sizeof(data_length)) {
            return false;
        }

        // Then send the actual data
        ssize_t bytes_sent = send(socket_fd, data.c_str(), data.size(), 0);
        return bytes_sent == static_cast<ssize_t>(data.size());
    }

    std::string Connection::receive_data(int socket_fd) {
        // Receive the length of the incoming data
        uint32_t data_length_network;
        ssize_t len = recv(socket_fd, &data_length_network, sizeof(data_length_network), 0);
        if (len <= 0) {
            return "";
        }
        uint32_t data_length = ntohl(data_length_network);

        // Receive the actual data
        std::string data;
        data.resize(data_length);
        size_t total_received = 0;
        while (total_received < data_length) {
            ssize_t bytes_received = recv(socket_fd, &data[total_received], data_length - total_received, 0);
            if (bytes_received <= 0) {
                return "";
            }
            total_received += bytes_received;
        }
        return data;
    }

}
