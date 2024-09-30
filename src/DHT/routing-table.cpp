#include "routing-table.h"
#include "utils.h" // For calculate_distance and other helper functions

namespace whispernet {

RoutingTable::RoutingTable(const std::string& local_node_id)
    : local_node_id(local_node_id) {
    // Initialize buckets
    buckets.resize(ID_BITS);
}

void RoutingTable::add_node(const Node& node) {
    if (node.get_id() == local_node_id) {
        // Do not add self to routing table
        return;
    }

    size_t bucket_index = get_bucket_index(node.get_id());

    std::lock_guard<std::mutex> lock(table_mutex);
    buckets[bucket_index].add_node(node);
}

std::vector<Node> RoutingTable::find_closest_nodes(const std::string& target_id, size_t num_nodes) {
    std::vector<Node> result;
    std::lock_guard<std::mutex> lock(table_mutex);

    // Create a min-heap or priority queue to sort nodes by distance
    auto compare = [&](const Node& a, const Node& b) {
        return compare_distance(target_id, a.get_id(), b.get_id());
    };

    std::priority_queue<Node, std::vector<Node>, decltype(compare)> node_queue(compare);

    // Add all nodes from the buckets to the queue
    for (const auto& bucket : buckets) {
        for (const auto& node : bucket.get_nodes()) {
            node_queue.push(node);
        }
    }

    // Extract the closest nodes
    while (!node_queue.empty() && result.size() < num_nodes) {
        result.push_back(node_queue.top());
        node_queue.pop();
    }

    return result;
}

size_t RoutingTable::get_bucket_index(const std::string& node_id) const {
    // Calculate the XOR distance
    std::vector<unsigned char> distance = calculate_distance(local_node_id, node_id);

    // Find the index of the first non-zero bit
    size_t index = 0;
    for (unsigned char byte : distance) {
        if (byte == 0) {
            index += 8;
        } else {
            for (int i = 7; i >= 0; --i) {
                if (byte & (1 << i)) {
                    return index;
                }
                ++index;
            }
        }
    }
    // If all bits are zero (same ID), return the maximum index
    return ID_BITS - 1;
}

} // namespace whispernet
