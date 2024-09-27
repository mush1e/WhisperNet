#include "node.h"

namespace whispernet {

    // Constructor
    Node::Node(const std::string& id, const std::string& username, const std::string& ip, int port)
        : id(id), username(username), ip(ip), port(port) {}

    // Getters
    std::string Node::get_id() const            { return id; }
    std::string Node::get_username() const      { return username; }
    std::string Node::get_IP() const            { return ip; }
    int Node::get_port() const                  { return port; }

    std::bitset<256> Node::calculate_xor_distance(const std::string& other_id) const {
        std::bitset<256> id_bits(id);
        std::bitset<256> other_id_bits(other_id);
        return id_bits ^ other_id_bits;
    }

} 
