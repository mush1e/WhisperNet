#include "routing-table.h"

namespace whispernet {
    RoutingTable::RoutingTable(const std::string& node_id) : node_id(node_id){
        buckets.resize(256);
    }

    unsigned int RoutingTable::xor_distance(const std::string& id1, const std::string& id2) const {
    std::string bin_id1 = hex_to_bin(id1);
    std::string bin_id2 = hex_to_bin(id2);
    if (bin_id1.size() != bin_id2.size()) {
        throw std::invalid_argument("IDs must have the same length");
    }
    std::bitset<256> bits1(bin_id1);  
    std::bitset<256> bits2(bin_id2);
    std::bitset<256> distance = bits1 ^ bits2;
    return distance.count();  
}

    int RoutingTable::get_bucket_index(const std::string& other_node_id) const {
        unsigned int distance = xor_distance(node_id, other_node_id);
        return static_cast<int>(log2(distance));  
    }

    void RoutingTable::add_node(const DHTNode& node) {
        int bucket_index = get_bucket_index(node.get_id());  // Get the appropriate bucket index
        if (bucket_index < buckets.size()) {
            buckets[bucket_index].push_back(node); 
        } else {
            std::cerr << "Invalid bucket index!" << std::endl;
        }
    }

    std::vector<DHTNode> RoutingTable::find_closest_nodes(const std::string& target_id, int num_nodes) {
        std::vector<DHTNode> closest_nodes;
        int bucket_index = get_bucket_index(target_id);  
        for (int i = bucket_index; i < buckets.size() && closest_nodes.size() < num_nodes; ++i) {
            for (const auto& node : buckets[i]) {
                closest_nodes.push_back(node);
                if (closest_nodes.size() >= num_nodes) {
                    break;
                }
            }
        }
        for (int i = bucket_index - 1; i >= 0 && closest_nodes.size() < num_nodes; --i) {
            for (const auto& node : buckets[i]) {
                closest_nodes.push_back(node);
                if (closest_nodes.size() >= num_nodes) {
                    break;
                }
            }
        }

        return closest_nodes;
    }
}