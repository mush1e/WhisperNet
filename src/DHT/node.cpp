#include "dht-node.h"

namespace whispernet {

    DHTNode::DHTNode(const std::string& id, const std::string& ip, int port) 
        : id(id), ip(ip), port(port), last_seen(std::time(nullptr)) {}
    
    const std::string& DHTNode::get_id() const {
        return id;
    }

    const std::string& DHTNode::get_ip() const {
        return ip;
    }

    std::time_t DHTNode::get_last_seen() const {
        return last_seen;
    }

    int DHTNode::get_port() const {
        return port;
    }

    void DHTNode::update_last_seen() {
        last_seen = std::time(nullptr);
    }

}