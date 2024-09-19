#include <iostream>

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
    return 0;
}
