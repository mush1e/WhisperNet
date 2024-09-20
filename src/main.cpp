#include <iostream>
#include "did.h"

void printWelcomeMessage() {
    std::cout << R"(
        .-. . .-..-. .-..-. .----.----..----..----. .-. .-..----.---. 
        | |/ \| || {_} || |{ {__ | {}  } {_  | {}  }|  `| || {_{_   _}
        |  .'.  || { } || |.-._} } .--'| {__ | .-. \| |\  || {__ | |  
        `-'   `-'`-' `-'`-'`----'`-'   `----'`-' `-'`-' `-'`----'`-' 
    )" << std::endl;
    std::cout << "Stay hidden\n";
}

int main() {
    printWelcomeMessage();
    DID nodeDID;

    // Display the public key (DID)
    std::cout << "Node DID (Public Key):" << std::endl;
    std::cout << nodeDID.getPublicKey() << std::endl;

    return 0;
}
