#ifndef DID_H
#define DID_H

#include <string>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <iostream>
#include <fstream>


class DID {
public:
    DID();
    ~DID();

    std::string getPublicKey() const;
    std::string getPrivateKey() const;
    std::string getUsername() const;

    void setUsername(const std::string& uname);
    void saveKeysToFile(const std::string& filename) const;
    bool loadKeysFromFile(const std::string& filename);

private:
    std::string publicKey;
    std::string privateKey;
    std::string username;

    void generateKeyPair();
};

#endif