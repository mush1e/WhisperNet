#include "keygen.h"

namespace whispernet {

    KeyManager::KeyManager() {
        OpenSSL_add_all_algorithms();
        ERR_load_crypto_strings();
    }

    void KeyManager::generate_keypair(const std::string& username) {
        EVP_PKEY* pkey = nullptr;                // The EVP_PKEY structure that holds both keys
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr); // Context for RSA key generation

        if (!ctx) {
            std::cerr << "Error initializing key context!" << std::endl;
            return;
        }

        if (EVP_PKEY_keygen_init(ctx) <= 0) {   
            std::cerr << "Error initializing key generation!" << std::endl;
            EVP_PKEY_CTX_free(ctx);
            return;
        }

        if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
            std::cerr << "Error setting key length!" << std::endl;
            EVP_PKEY_CTX_free(ctx);
            return;
        }

        if (EVP_PKEY_keygen(ctx, &pkey) <= 0) { 
            std::cerr << "Error generating key pair!" << std::endl;
            EVP_PKEY_CTX_free(ctx);
            return;
        }

        EVP_PKEY_CTX_free(ctx);   

        if (!pkey) {
            std::cerr << "Failed to generate key pair!" << std::endl;
            return;
        }

        if (!save_private_key(pkey, "private_key.pem")) {
            std::cerr << "Failed to save private key!" << std::endl;
        }

        if (!save_public_key(pkey, "public_key.pem")) {
            std::cerr << "Failed to save public key!" << std::endl;
        }

        if (!save_username(username)) {
            std::cerr << "Failed to save username!" << std::endl;
        }

        EVP_PKEY_free(pkey);
    }

    bool KeyManager::save_private_key(EVP_PKEY* pkey, const std::string& filename) {
        FILE* private_key_file = fopen(filename.c_str(), "wb");
        if (!private_key_file) {
            std::cerr << "Error opening file for private key!" << std::endl;
            return false;
        }
        if (!PEM_write_PrivateKey(private_key_file, pkey, nullptr, nullptr, 0, nullptr, nullptr)) {
            std::cerr << "Error writing private key to file!" << std::endl;
            fclose(private_key_file);
            return false;
        }
        fclose(private_key_file);
        return true;
    }

    bool KeyManager::save_public_key(EVP_PKEY* pkey, const std::string& filename) {
        FILE* public_key_file = fopen(filename.c_str(), "wb");
        if (!public_key_file) {
            std::cerr << "Error opening file for public key!" << std::endl;
            return false;
        }
        if (!PEM_write_PUBKEY(public_key_file, pkey)) {
            std::cerr << "Error writing public key to file!" << std::endl;
            fclose(public_key_file);
            return false;
        }
        fclose(public_key_file);
        return true;
    }  

    bool KeyManager::save_username(const std::string& username) {
        std::ofstream config_file("./config.txt");
        if (!config_file.is_open()) {
            std::cerr << "Failed to open config file for writing." << std::endl;
            return false;
        }

        config_file << "username = " << username << std::endl;
        config_file.close();
        return true;
    }

    std::string KeyManager::get_username() const{
        std::ifstream config_file("./config.txt");
        if (!config_file.is_open()) {
            std::cerr << "Failed to open config file." << std::endl;
            return "";
        }
        std::string line, username;
        while (std::getline(config_file, line)) {
            if (line.find("username") == 0) {
                username = line.substr(line.find("=") + 2);
                break;
            }
        }
        config_file.close();
        return username;
    }

    KeyManager::operator bool() const {
        std::ifstream priv_key_file("private_key.pem");
        std::ifstream pub_key_file("public_key.pem");
        return priv_key_file.is_open() && pub_key_file.is_open();
    }

    KeyManager::~KeyManager() {
        EVP_cleanup();  
        ERR_free_strings();
    }

}