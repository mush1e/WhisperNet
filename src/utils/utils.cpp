#include "utils.h"

namespace whispernet {

    std::string display_welcome() {
        return  R"(
        .-. . .-..-. .-..-. .----.----..----..----. .-. .-..----.---. 
        | |/ \| || {_} || |{ {__ | {}  } {_  | {}  }|  `| || {_{_   _}
        |  .'.  || { } || |.-._} } .--'| {__ | .-. \| |\  || {__ | |  
        `-'   `-'`-' `-'`-'`----'`-'   `----'`-' `-'`-' `-'`----'`-' 
        )";
    }

    std::string hash_username(const std::string& username) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, username.c_str(), username.size());
        SHA256_Final(hash, &sha256);

        std::stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }

    std::string hex_char_to_bin(char c) {
        switch (toupper(c)) {
            case '0': return "0000";
            case '1': return "0001";
            case '2': return "0010";
            case '3': return "0011";
            case '4': return "0100";
            case '5': return "0101";
            case '6': return "0110";
            case '7': return "0111";
            case '8': return "1000";
            case '9': return "1001";
            case 'A': return "1010";
            case 'B': return "1011";
            case 'C': return "1100";
            case 'D': return "1101";
            case 'E': return "1110";
            case 'F': return "1111";
            default:
                throw std::invalid_argument("Invalid hex character");
        }
    }

    // Convert a full hexadecimal string to a binary string
    std::string hex_to_bin(const std::string& hex) {
        std::string bin;
        for (char c : hex) {
            bin += hex_char_to_bin(c);
        }
        return bin;
    }

}