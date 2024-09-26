#include "routing_table.h"

namespace whispernet {

    void RoutingTable::add_node(const Node& node) {
         node_map_by_id[node.get_id()] = node;
        node_map_by_username[node.get_username()] = node;
    }

    Node* RoutingTable::find_node_by_id(const std::string& id) {
        if (node_map_by_id.find(id) != node_map_by_id.end()) {
            return &node_map_by_id[id];
        }
        return nullptr; 
    }

    Node* RoutingTable::find_node_by_username(const std::string& username) {
        if (node_map_by_username.find(username) != node_map_by_username.end()) {
            return &node_map_by_username[username];
        }
        return nullptr;
    }

}