#include "did.h"

DID::DID() {
    if (!loadKeysFromFile("node_keys.pem")) {
        std::cout << "No existing keys found, generating new key pair." << std::endl;
        generateKeyPair();
        saveKeysToFile("node_keys.pem");
    } else {
        std::cout << "Loaded keys from file." << std::endl;
    }
}

DID::~DID() {}

void DID::generateKeyPair() {
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!ctx) {
        throw std::runtime_error("Error creating EVP_PKEY_CTX.");
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        throw std::runtime_error("Error initializing key generation.");
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
        throw std::runtime_error("Error setting RSA key length.");
    }

    EVP_PKEY *pkey = nullptr;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        throw std::runtime_error("Error generating key pair.");
    }

    BIO *pri = BIO_new(BIO_s_mem());
    BIO *pub = BIO_new(BIO_s_mem());

    PEM_write_bio_PrivateKey(pri, pkey, nullptr, nullptr, 0, nullptr, nullptr);
    PEM_write_bio_PUBKEY(pub, pkey);

    char *priKey = nullptr;
    long priLen = BIO_get_mem_data(pri, &priKey);
    privateKey = std::string(priKey, priLen);

    char *pubKey = nullptr;
    long pubLen = BIO_get_mem_data(pub, &pubKey);
    publicKey = std::string(pubKey, pubLen);

    std::cout << "Enter a username for this node: ";
    std::getline(std::cin, username);

    BIO_free_all(pri);
    BIO_free_all(pub);
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
}

std::string DID::getPublicKey() const {
    return publicKey;
}

std::string DID::getPrivateKey() const {
    return privateKey;
}

void DID::saveKeysToFile(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile) {
        throw std::runtime_error("Unable to open file for writing: " + filename);
    }

    outFile << "Username: " << username << "\n";
    outFile << privateKey;
    outFile << publicKey;
    outFile.close();
}

bool DID::loadKeysFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile) {
        return false; // No file found
    }

    std::string line, key;
    bool isPrivate = false, isPublic = false;

    while (std::getline(inFile, line)) {
        if (line.rfind("Username:", 0) == 0) {
            username = line.substr(9);
            continue;
        }
        if (line == "-----BEGIN PRIVATE KEY-----") {
            isPrivate = true;
            continue;
        }
        if (line == "-----END PRIVATE KEY-----") {
            isPrivate = false;
            privateKey = key;
            key.clear();
            continue;
        }
        if (line == "-----BEGIN PUBLIC KEY-----") {
            isPublic = true;
            continue;
        }
        if (line == "-----END PUBLIC KEY-----") {
            isPublic = false;
            publicKey = key;
            key.clear();
            continue;
        }

        if (isPrivate || isPublic) {
            key += line + "\n";
        }
    }

    inFile.close();
    return true;
}
