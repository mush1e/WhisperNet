#include <iostream>
#include "keygen.h"
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

    return 0;
}
