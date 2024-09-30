#pragma once

#include <string>
#include <ctime>
#include <vector>
#include <list>

namespace whispernet {

    class DHTNode {
    public:
        DHTNode(const std::string& id, const std::string& ip, int port);
        
        const std::string& get_id() const;
        const std::string& get_ip() const;
        std::time_t get_last_seen() const;
        int get_port() const;
        
        void update_last_seen();

    private:
        std::string id;  
        std::string ip;  
        int port;         
        std::time_t last_seen; 
    };
}
