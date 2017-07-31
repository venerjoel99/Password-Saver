#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED
#include <fstream>
#include <string>
#include "Encryptor.h"

class Info{
private:
    std::string website, username, password;
public:
    std::string getWebsite(){return website;}
    std::string getUsername(){return username;}
    std::string getPassword(){return password;}
    Info(void){}
    Info(std::string, std::string, std::string);
    static std::string input();         //Inputs user entry into the class object
    std::string toStr();                //Displays the things in the classes
};

///Writes credential objects into text file
std::fstream& operator<<(std::fstream&, Info);

///Converts text file content to string
std::fstream& operator>>(std::fstream&, std::string&);

class Data{
private:
    //const std::string dummy = "key0.bin";
    std::string fileName;
    std::string fileDir;
    std::string keyword;
    std::fstream theFile;
    enum OpenMode{
        REGULAR,
        REWRITE,
        ENCRYPTION,
        READ_ONLY,
        CLOSED
    }currentMode=CLOSED;
    int searchEngine(std::string);                     //Searches for the keyword
    Info read(int);
public:
    bool open(OpenMode);
    void close();
    enum Success{
        SUCCESS,
        FILE_NOT_FOUND,
        NO_RESULTS
    };
    Data();
    Data(std::string);
    Data(std::string, std::string);
    virtual ~Data();
    bool isOpen();
    bool is_encrypted();                    //Checks if file is encrypted
    int findSize();            //Find size of a file in the stream
    Info findSearch(std::string);
    Data::Success writeIntoFile(Info);   //writes credential into file
    Data::Success readFromFile(std::string&);        //read content from the file
    Data::Success changeFile(std::string, std::string);
    Data::Success changeFile(std::string, bool);
    enum Change{
        US,
        PS,
        DE
    };
    Data::Success changeInfo(std::string ,std::string , Data::Change);
    Data::Success changeInfo(std::string, std::string, std::string);
    Encryptor::Status encrypt(bool, std::string);
    Encryptor::Status decrypt(std::string);
    std::string getFileName();
    void encrypt(bool);
    bool isEncrypted();
};

#endif // DATA_H_INCLUDED
