#pragma once

#include <openssl/sha.h>
#include <string>
#include <bitset>
#include <sstream>
#include <iomanip>
#include <string>



namespace whispernet {
    
    std::string display_welcome();
    std::string hash_username(const std::string& username);
    std::string hex_char_to_bin(char c);
    std::string hex_to_bin(const std::string& hex);
}