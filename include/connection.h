#pragma once

#include <string>
#include <map>
#include <mutex>
#include <thread>

namespace whispernet {

class Connection {
public:
    Connection(const std::string& username);
    ~Connection();

    bool start_server(int port);
    bool send_connection_request(const std::string& target_username);
    void handle_incoming_requests();

    void send_message(const std::string& target_username, const std::string& message);

private:
    int server_socket;
    std::string username;

    std::mutex peers_mutex;
    std::map<std::string, int> connected_peers; // username -> socket_fd

    void accept_connections();
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

