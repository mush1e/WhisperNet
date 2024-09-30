#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>


namespace whispernet {

    class KeyManager {
    public:
        KeyManager();
        ~KeyManager();
        void generate_keypair(const std::string& username);
        std::string get_username() const; 
        operator bool() const;
    private:
        bool save_private_key(EVP_PKEY* pkey, const std::string& filename);
        bool save_public_key(EVP_PKEY* pkey, const std::string& filename);
        bool save_username(const std::string& username);
    };

}