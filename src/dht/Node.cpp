#include "Node.h"

Node::Node(const std::string& id) : id(id) {}

void Node::add_peer(const std::string& peer_id) {
    peers.push_back(peer_id);
}

const std::vector<std::string>& Node::get_peers() const {
    return peers;
}

void Node::store(const std::string& key, const std::string& value) {
    storage[key] = value;
}