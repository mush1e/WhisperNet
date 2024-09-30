#pragma once
#include <list>
#include <iostream>
#include <vector>
#include <cmath>
#include <bitset>

#include "dht-node.h"
#include "utils.h"

namespace whispernet {

    class RoutingTable {
    public:
        RoutingTable(const std::string& node_id);
        void add_node(const DHTNode& node);
        std::vector<DHTNode> find_closest_nodes(const std::string& target_id, int num_nodes = 5);

    private:
        std::string node_id;  
        std::vector<std::list<DHTNode>> buckets;
        unsigned int xor_distance(const std::string& id1, const std::string& id2) const;
        int get_bucket_index(const std::string& node_id) const;
    };
}
