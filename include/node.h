#pragma once

#include <string>
#include <bitset>

namespace whispernet {

    class Node {
    public:
        Node(const std::string& id, const std::string& username, const std::string& ip, int port);

        std::string get_id() const;
        std::string get_username() const;
        std::string get_IP() const;
        int get_port() const;

        std::bitset<256> calculate_xor_distance(const std::string& other_id) const;
    
    private:
        std::string id;        // Node's DID (public key)
        std::string username;  // Node's username
        std::string ip;        // Node's IP address
        int port;              // Node's port
    };

}