#include "utils.h"
#include <sstream>
#include <iomanip>

namespace whispernet {

std::vector<unsigned char> calculate_distance(const std::string& id1, const std::string& id2) {
    // Assuming id1 and id2 are hex strings
    std::vector<unsigned char> bytes1(id1.length() / 2);
    std::vector<unsigned char> bytes2(id2.length() / 2);

    for (size_t i = 0; i < id1.length(); i += 2) {
        bytes1[i / 2] = std::stoi(id1.substr(i, 2), nullptr, 16);
        bytes2[i / 2] = std::stoi(id2.substr(i, 2), nullptr, 16);
    }

    std::vector<unsigned char> distance(bytes1.size());
    for (size_t i = 0; i < bytes1.size(); ++i) {
        distance[i] = bytes1[i] ^ bytes2[i];
    }

    return distance;
}

bool compare_distance(const std::string& target_id, const std::string& id1, const std::string& id2) {
    std::vector<unsigned char> dist1 = calculate_distance(target_id, id1);
    std::vector<unsigned char> dist2 = calculate_distance(target_id, id2);

    for (size_t i = 0; i < dist1.size(); ++i) {
        if (dist1[i] < dist2[i]) {
            return true;
        } else if (dist1[i] > dist2[i]) {
            return false;
        }
    }
    return false;
}

} 
