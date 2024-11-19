#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class Node {
public:
    Node(const std::string& id);
    void add_peer(const std::string& peer_id);
    const std::vector<std::string>& get_peers() const;
    void store(const std::string& key, const std::string& value);
    std::string retreive(const std::string& key) const;
private:
    std::string id {};
    std::vector<std::string> peers {};
    std::unordered_map<std::string, std::string> storage {};
};