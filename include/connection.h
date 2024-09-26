#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <mutex>
#include <thread>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <google/protobuf/stubs/common.h> 
#include <random>

#include "did.h"
namespace whispernet {

class Connection {
public:
    Connection(const std::string& username, const DID& did);
    ~Connection();

    bool start_server(int port);
    bool send_connection_request(const std::string& target_username);
    void handle_incoming_requests();
    void accept_connections();

    void send_message(const std::string& target_username, const std::string& message);

private:
    int server_socket;
    std::string username;
    DID did;

    std::mutex peers_mutex;
    std::unordered_map<std::string, int> connected_peers; // username -> socket_fd
    bool get_peer_info(const std::string& username, std::string& ip, int& port);
    void process_incoming_request(int socket_fd);
    void receive_messages(int socket_fd, const std::string& peer_username);

    bool send_data(int socket_fd, const std::string& data);
    std::string receive_data(int socket_fd);

    void close_connection();

    std::string lookup_did_in_dht(const std::string& did);
    std::string generate_random_challenge();
    bool verify_signature(const std::string& signed_data, const std::string& original_data, const std::string& public_key);
    std::string sign_challenge(const std::string& challenge);
};

} // namespace whispernet

