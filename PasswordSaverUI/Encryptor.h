#ifndef ENCRYPTOR_H_INCLUDED
#define ENCRYPTOR_H_INCLUDED

#include <string>
#include <fstream>
#include "aes.h"

class Encryptor{
private:
    int keyLength = CryptoPP::AES::DEFAULT_KEYLENGTH;
    const int keySize1 = CryptoPP::AES::MIN_KEYLENGTH;
    const int keySize2 = (CryptoPP::AES::MIN_KEYLENGTH +
        CryptoPP::AES::MAX_KEYLENGTH) / 2;
    const int keySize3 = CryptoPP::AES::MAX_KEYLENGTH;
    const int blockSize = CryptoPP::AES::BLOCKSIZE;
    const int haystackSize = 10000;
    const int MIN_UNENCRYPTED = 0;
    const int MAX_UNENCRYPTED = 128;
    std::string directory;
    std::fstream keyStream;
    std::fstream mainStream;
    int findSize(std::fstream&);
    void generateHayStack(void);
    int generateKeyFile();
    void configureHaystack(int, int);
    std::string generateKey(int, int);
    std::string generateIV(int);
    bool openKey(bool);
    void open(bool, std::string);
    void open(std::string);
    void close(std::fstream&);
    std::string retrieveKey(std::string);
    std::string fileStr(std::string);
    int generatePIN(std::string);
public:
    Encryptor(void);
    Encryptor(std::string);
    ~Encryptor(){
        if (keyStream.is_open()) keyStream.close();
        if (mainStream.is_open()) mainStream.close();
    }
    enum Status{
        SUCCESS,
        FAIL,
        ENCRYPTED,
        DECRYPTED,
        WRONG_PASSWORD,
        KEY_NOT_FOUND
    };
    bool isEncrypted(std::string);
    bool isEncrypted(void);
    //bool isEncrypted(void);
private:
    Status encryptHaystack(std::string);
    Status decryptHaystack(std::string);
public:
    std::string encryptText(bool, std::string, std::string, std::string);
    Status encryptFile(bool, std::string, std::string, std::string);
    Status encrypt(std::string, std::string);
    Status decrypt(std::string, std::string);
};

#endif // ENCRYPTOR_H_INCLUDED
