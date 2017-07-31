#ifndef ENCRYPTOR_H_INCLUDED
#define ENCRYPTOR_H_INCLUDED
#include <fstream>
#include <string>

class Encryptor{
private:
    std::string directory;
    std::fstream keyStream;
    std::fstream mainStream;
    int findSize(std::fstream&);
    void generateHayStack(void);
    int generateKeyFile(bool);
    void configureHaystack(bool, int, int);
    std::string generateKey(bool, int, int);
    std::string generateIV(int);
    bool openKey(bool);
    void open(bool, std::string);
    void open(std::string);
    void close(std::fstream&);
    std::string retrieveKey(bool, bool, std::string);
    std::string fileStr(std::string);
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
    Status encryptHaystack(bool, std::string);
    Status decryptHaystack(std::string);
public:
    std::string encryptText(bool, std::string, std::string, std::string);
    Status encryptFile(bool, std::string, std::string, std::string);
    Status encrypt(bool, std::string, std::string);
    Status decrypt(std::string, std::string);
};

#endif // ENCRYPTOR_H_INCLUDED
