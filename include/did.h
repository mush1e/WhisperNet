#pragma once


#include <string>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <iostream>
#include <fstream>
#include <sstream>

namespace whispernet {
    class DID {
    public:
        DID(const std::string& username);
        ~DID();

        std::string getPublicKey() const;
        std::string getUsername() const;
        std::string getPrivateKey() const;

        void setUsername(const std::string& uname);
        void saveKeysToFile(const std::string& filename) const;
        bool loadKeysFromFile(const std::string& filename);
        std::string get_hashed_DID() const;


    private:
        std::string publicKey;
        std::string privateKey;
        std::string username;

        void generateKeyPair();
    };
}

