#include <iostream>
#include "keygen.h"
#include "routing-table.h"
#include "utils.h"

int main() {
    std::cout << whispernet::display_welcome() << std::endl
              << "Stay secure, Stay hidden...\n\n\n";
    
    whispernet::KeyManager km;

    if (!km) {
        std::string usr_name;
        std::cout << "~~~~~~~Welcome to whispernet!~~~~~~~\n" << "Pick a username : ";
        std::cin >> usr_name;
        km.generate_keypair(usr_name);
    } else {
        std::cout << "Welcome back " << km.get_username() << "!\n";
    }
// Hash the node IDs to use as valid hexadecimal IDs
    std::string current_node_id = whispernet::hash_username("current_node_id");
    std::string node1_id = whispernet::hash_username("node1_id");
    std::string node2_id = whispernet::hash_username("node2_id");
    std::string node3_id = whispernet::hash_username("node3_id");
    std::string target_node_id = whispernet::hash_username("target_node_id");

    whispernet::RoutingTable routing_table(current_node_id);

    whispernet::DHTNode node1(node1_id, "192.168.1.1", 8080);
    whispernet::DHTNode node2(node2_id, "192.168.1.2", 8080);
    whispernet::DHTNode node3(node3_id, "192.168.1.3", 8080);

    routing_table.add_node(node1);
    routing_table.add_node(node2);
    routing_table.add_node(node3);

    // Find the closest nodes to a target ID
    std::vector<whispernet::DHTNode> closest_nodes = routing_table.find_closest_nodes(target_node_id, 20);

    // Output the closest nodes
    for (const auto& node : closest_nodes) {
        std::cout << "Closest node: " << node.get_id() << " at " << node.get_ip() << ":" << node.get_port() << std::endl;
    }
    return 0;
}
