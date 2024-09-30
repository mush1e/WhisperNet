#include "node.h"

namespace whispernet {

    Node::Node(const std::string& id, const std::string& ip, int port, std::time_t last_seen) 
        : id(id), ip(ip), port(port), last_seen(last_seen) {}
    
    const std::string& Node::get_id() const {
        return this->id;
    }

    const std::string& Node::get_ip() const {
        return this->ip;
    }

    int Node::get_port() const {
        return this->port;
    }

    std::time_t Node::get_last_seen() const {
        return this->last_seen;
    }

    void Node::update_last_seen() {
        this->last_seen = std::time(nullptr);
    }

    bool Node::operator==(const Node& other) const {
        return id == other.id;
    }

}