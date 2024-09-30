#include "k-bucket.h"

namespace whispernet {
    KBucket::KBucket() {
        nodes.reserve(k);
    }

    void KBucket::add_node(const Node& node) {
        std::lock_guard<std::mutex> lock(bucket_mutex);
        auto it = std::find(nodes.begin(), nodes.end(), node);
        if (it != nodes.end()) {
            nodes.erase(it);
            nodes.push_back(node);
        } else {
            if (nodes.size() < k) {
                nodes.push_back(node);
            } else {
                nodes.erase(nodes.begin());
                nodes.push_back(node);
            }
        }
    }

    void KBucket::remove_node(const std::string& node_id) {
        std::lock_guard<std::mutex> lock(bucket_mutex);
        nodes.erase(
        std::remove_if(
            nodes.begin(),
            nodes.end(),
            [&node_id](const Node& node) { return node.get_id() == node_id; }),
        nodes.end());
    }

    bool KBucket::contains(const std::string& node_id) const {
        std::lock_guard<std::mutex> lock(bucket_mutex);
        return std::any_of(
            nodes.begin(),
            nodes.end(),
            [&node_id](const Node& node) { return node.get_id() == node_id; });
    }

    std::vector<Node> KBucket::get_nodes() const {
        std::lock_guard<std::mutex> lock(bucket_mutex);
        return nodes; 
    }
}