#pragma once

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <map>
#include <mutex>
#include <thread>

namespace whispernet {

    class Connection {
    public:
        Connection(const std::string& username);
        ~Connection(); 

        bool start_server(int port);
        bool send_connection_request(const std::string& username);
        void handle_incoming_request();
        void sebd_message(const std::string& target_username, const std::string& msg);
    
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
    };

}

