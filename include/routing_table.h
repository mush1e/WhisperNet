#pragma once

#include <unordered_map>
#include <string>
#include "node.h"

namespace whispernet {

    class RoutingTable {
    public:
        void add_node(const Node& node);
        Node* find_node_by_id(const std::string& id);
        Node* find_node_by_username(const std::string& username);
    private:
        std::unordered_map<std::string, Node> node_map_by_id;           // DID -> Node
        std::unordered_map<std::string, Node> node_map_by_username;     // DID -> username
    };

}