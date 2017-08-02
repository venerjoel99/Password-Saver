#include "Encryptor.h"
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <iostream>

#include "modes.h"
#include "aes.h"
#include "filters.h"
#include "Convert.h"

/**
 * Default constructor
 */
Encryptor::Encryptor(){
    directory = "";
}

/**
 * Constructor with key directory passed
 */
Encryptor::Encryptor(std::string keyDir){
    directory = keyDir;
};

/**
 * Opens key.bin
 * @param write - true if open for write-only, false if open for reading
 */
void Encryptor::openKey(bool write){
    std::string keyLocation = directory + "key.bin";
    if (keyStream.is_open()) keyStream.close();
    if (write){
        keyStream.open(keyLocation.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
    }
    else{
        keyStream.open(keyLocation.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
    }
    if (!keyStream.is_open()) throw Encryptor::KEY_NOT_FOUND;
}

/**
 * Opens a haystack file for write or read mode
 * @param write - write(true) or read(false) mode
 * @param fileName - full file name (directory, file, and extension)
 */
void Encryptor::open(bool write, int fileNum){
    std::string fileName = directory + "key" + Convert::intStr(fileNum) + ".bin";
    if (mainStream.is_open()) mainStream.close();
    if (write){
        mainStream.open(fileName.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
    }
    else{
        mainStream.open(fileName.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
    }
    if (!mainStream.is_open()) throw Encryptor::FILE_NOT_FOUND;
}

/**
 * Opens a haystack file for both write and read operation
 * @param fileName - full file name
 */
void Encryptor::open(int fileNum){
    std::string fileName = directory + "key" + Convert::intStr(fileNum) + ".bin";
    if (mainStream.is_open()) mainStream.close();
    mainStream.open(fileName.c_str(), std::ios::out|std::ios::binary|std::ios::in);
    if (!mainStream.is_open()) throw Encryptor::FILE_NOT_FOUND;
}

/**
 * Opens a specified file for write or read mode
 * @param write - write(true) or read(false) mode
 * @param fileName - full file name (directory, file, and extension)
 */
void Encryptor::open(bool write, std::string fileName){
    if (mainStream.is_open()) mainStream.close();
    if (write){
        mainStream.open(fileName.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
    }
    else{
        mainStream.open(fileName.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
    }
    if (!mainStream.is_open()) throw Encryptor::FILE_NOT_FOUND;
}

/**
 * Opens a specified file for both write and read operation
 * @param fileName - full file name
 */
void Encryptor::open(std::string fileName){
    if (mainStream.is_open()) mainStream.close();
    mainStream.open(fileName.c_str(), std::ios::out|std::ios::binary|std::ios::in);
    if (!mainStream.is_open()) throw Encryptor::FILE_NOT_FOUND;
}

/**
 * Closes a specified file stream if it is open
 * @param stream - the file stream object
 */
void Encryptor::close(std::fstream& stream){
    if (stream.is_open()) stream.close();
}

/**
 * Generates a PIN from a user-entered password
 * @param password - the user-entered password string
 * @return the generated PIN number(0000 to 9999)
 */
int Encryptor::generatePIN(std::string password){
    try{
        openKey(false);
    }
    catch(Encryptor::Status s){
        std::cout << "here1\n";
        throw s;
    }
    char begKeyChar, endKeyChar, begChar, endChar;
    keyStream.seekg(0, std::ios::beg);
    keyStream.get(begKeyChar);
    keyStream.seekg(keyLength - 1, std::ios::beg);
    keyStream.get(endKeyChar);
    begChar = password.at(0);
    int size = password.size();
    endChar = password.at(size - 1);
    int PIN = 0;
    PIN += (((int)pow((int)begKeyChar, 2.0) ^ (int)pow((int)begChar, 2.0)) % 10) * 1000;
    PIN += (((int)pow((int)endKeyChar, 2.0) ^ (int)pow((int)endChar, 2.0)) % 10) * 100;
    PIN += (((int)pow((int)begKeyChar, 2.0) ^ (int)pow((int)endKeyChar, 2.0)) % 10) * 10;
    PIN += ((int)pow(2.0, size) ^ (int)pow(size, 3.0)) % 10;
    return PIN;
}

/**
 * Create 3 key[num].bin files(see readme)
 */
void Encryptor::generateHayStack(){
    srand(time(0));
    for (int i = 1; i < 4; i++){
        try{
            open(true, i);
        }
        catch(Encryptor::Status s){
            std::cout << "here2\n";
            throw s;
        }
        int fileSize = haystackSize + blockSize;
        char passcode[fileSize + 8];
        for (int i=0; i < fileSize; i++){
            int random = rand()%93;
            passcode[i] = '!' + random;
        }
        for (int i=fileSize; i < fileSize + 8; i++){
            int random = rand()%10;
            char num = Convert::intChar(random);
            passcode[i] = num;
        }
        mainStream.write(passcode, 10024);
        close(mainStream);
    }
}

/**
 * Create a binary file with the key
 * to encrypt/decrypt the key[num].bin files(see readme)
 * @return the key file number to be configured for encryption
 */
int Encryptor::generateKeyFile(){
    char primaryKey[keyLength];
    srand(time(0));
    for (int i = 0; i < keyLength; i++){
        int random = rand()%93;
        primaryKey[i] = '!' + random;
    }
    try{
        openKey(true);
    }
    catch(Encryptor::Status s){
        std::cout << "here3\n";
        throw s;
    }
    keyStream.seekp(0, std::ios::beg);
    keyStream.write(primaryKey, keyLength);
    close(keyStream);
    return ((int)primaryKey[0]) % 3 + 1;
}

/**
 * Return the content of the file
 * in the form of a std::string object
 * @param fileName - full file name
 * @return the file's content in string
 */
std::string Encryptor::fileStr(std::string fileName){
    std::string fileString = "";
    char stringCreator;
    try{
        open(false, fileName);
    }
    catch(Encryptor::Status s){
        std::cout << "here4\n";
        throw s;
    }
    int fileSize = findSize(mainStream);
    for (int i = 0; i < fileSize; i++){
        mainStream.seekg(i);
        mainStream.get(stringCreator);
        fileString += stringCreator;
    }
    close(mainStream);
    return fileString;
}

/**
 * Configres one of the key[num].bin file to be used
 * for one time encryption and decryption(see readme)
 * @param PIN - password-generated PIN
 * @param fileNum - the number of the key[num].bin file to be used
 */
void Encryptor::configureHaystack(int PIN, int fileNum){
    try{
        open(fileNum);
    }
    catch(Encryptor::Status s){
        throw s;
    }
    srand(time(0));
    int ivNum = 0;
    int place = 1000;
    int fileSize = haystackSize + blockSize;
    for (int i=fileSize; i < fileSize + 8; i+=2){
        int random = rand()%10;
        ivNum += (random * place);
        place /= 10;
        mainStream.seekp(i, std::ios::beg);
        mainStream.put(Convert::intChar(random));
    }
    int randPIN = rand()%10000;
    int arbNum = Convert::bitwiseXOR(PIN, randPIN);
    place = 1000;
    for (int i=fileSize + 1; i < fileSize + 8; i+=2){
        char num;
        num = Convert::intChar(Convert::digit(arbNum, place));
        place /= 10;
        mainStream.seekp(i, std::ios::beg);
        mainStream.put(num);
    }
    close(mainStream);
}

/**
 * Generate the main file IV(see readme)
 * @param fileNum the number of the key[num].bin file
 * @return the IV std::string to be used for Enc/Dec
 */
std::string Encryptor::generateIV(int fileNum){
    std::string ivLoc;
    try{
        open(false, fileNum);
    }
    catch(Encryptor::Status s){
        std::cout << "here5\n";
        throw s;
    }
    int fileSize = haystackSize + blockSize;
    for (int i = fileSize; i < fileSize + 8; i+=2){
        char digit;
        mainStream.seekg(i, std::ios::beg);
        mainStream.get(digit);
        if (i==0) ivLoc=digit;
        else ivLoc+=digit;
    }
    int ivPin = atoi(ivLoc.c_str());
    mainStream.seekg(ivPin, std::ios::beg);
    char finalIV[blockSize];
    mainStream.read(finalIV, blockSize);
    close(mainStream);
    std::string ivStr = "";
    for (unsigned int i = 0; i < blockSize; i++){
        ivStr += finalIV[i];
    }
    return ivStr;
}

/**
 * Generate the main file key(see readme)
 * @param PIN - generated by a password in another function
 * @param fileNum - the number of the key[num].bin file
 * @return the generated key
 */
std::string Encryptor::generateKey(int PIN, int fileNum){
    try{
        open(false, fileNum);
    }
    catch(Encryptor::Status s){
        std::cout << "here6\n";
        throw s;
    }
    int arbPIN, indexPIN;
    std::string pinStr = "";
    int fileSize = haystackSize + blockSize;
    for (int i = fileSize + 1; i < fileSize + 8; i+=2){
        char num;
        mainStream.seekg(i, std::ios::beg);
        mainStream.get(num);
        pinStr+=num;
    }
    arbPIN = atoi(pinStr.c_str());
    indexPIN = Convert::bitwiseXOR(PIN, arbPIN);
    int place = 1000;
    int place2 = 1000;
    char draftKey[keyLength / 4][4];
    srand(time(0));
    for (int i=0; i < keyLength / 4; i++){
        int index = (place==0) ?
            (indexPIN + Convert::digit(PIN, place2)) :
            (indexPIN + Convert::digit(arbPIN, place));
        mainStream.seekg(index, std::ios::beg);
        mainStream.read(draftKey[i], 4);
        if (place!=0) place /= 10;
        else place2 /= 10;
    }
    char finalKey[keyLength];
    for (int i=0; i < (keyLength/4); i++){
        for (int j=0; j<4; j++){
            finalKey[(4*i)+j] = draftKey[i][j];
        }
    }
    close(mainStream);
    std::string keyStr = "";
    for (int i=0; i < keyLength; i++){
        keyStr += finalKey[i];
    }
    return keyStr;
}

/**
 * Checks if the haystack files themselves are all encrypted or not
 * by checking if each character's integer value is between 0 and 127
 * @return whether all 3 files are encyrpted or not
 */
bool Encryptor::isEncrypted(){
    bool encrypted[3];
    for (int i = 0; i < 3; i++){
        std::string fileName = directory + "key" + Convert::intStr(i+1) + ".bin";
        std::string dataFeed;
        try{
            fileStr(fileName);
        }catch(Encryptor::Status s){
            std::cout << "here7\n";
            return false;
        }
        int fileSize = dataFeed.size();
        char* charChecker = new char[fileSize];
        for (int j=0; j<fileSize; j++){
            charChecker[j] = dataFeed.at(j);
        }
        encrypted[i] = false;
        for (int k=0; k<fileSize; k++){
            if ((int)charChecker[k]< MIN_UNENCRYPTED ||
                (int)charChecker[k]> MAX_UNENCRYPTED){
                delete[] charChecker;
                encrypted[i] = true;
                break;
            }
        }
        delete[] charChecker;
    }
    return (encrypted[0] && encrypted[1] && encrypted[2]);
}

/**
 * Checks if the file is encrypted by checking each integer value
 * of each character in the file
 * @param fileName - the full file location
 * @return whether the file is encrypted or not
 */
bool Encryptor::isEncrypted(std::string fileName){
    try{
        open(false, fileName);
    }
    catch(Encryptor::Status s){
        std::cout << "here8\n";
        return false;
    }
    std::string dataFeed = fileStr(fileName);
    int fileSize = dataFeed.size();
    char* charChecker = new char[fileSize];
    for (int i=0; i<fileSize; i++){
        charChecker[i] = dataFeed.at(i);
    }
    for (int i=0; i<fileSize; i++){
        if ((int)charChecker[i] < MIN_UNENCRYPTED ||
            (int)charChecker[i] > MAX_UNENCRYPTED){
            delete[] charChecker;
            return true;
        }
    }
    delete[] charChecker;
    return false;
}

/**
 * Encrypt the text
 * @param encrypt - encrypt(true) or decrypt(false)
 * @param text - plain or cipher text
 * @param ivStr - the IV string
 * @param keyStr - the Key string
 * @return the resulting string from encryption/decryption
 */
std::string Encryptor::encryptText(bool encrypt, std::string text, std::string ivStr, std::string keyStr){
    std::string plaintext, ciphertext;
    char keyArray[keyStr.size()], ivArray[ivStr.size()];
    for (unsigned int i = 0; i < keyStr.size(); i++){
        keyArray[i] = keyStr.at(i);
    }
    for (unsigned int i = 0; i < ivStr.size(); i++){
        ivArray[i] = ivStr.at(i);
    }
    byte key[ keyLength ], iv[ blockSize ];
    memmove(key, keyArray,  keyLength);
    memmove(iv, ivArray, blockSize);
    ///Source of the following code: https://stackoverflow.com/questions/12306956/example-of-aes-using-crypto
    ///Begin cited code
    if (encrypt){
        plaintext = text;
        CryptoPP::AES::Encryption aesEncryption(key, keyLength);
        CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );
        CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink( ciphertext ) );
        stfEncryptor.Put( reinterpret_cast<const unsigned char*>( plaintext.c_str() ), plaintext.length() + 1 );
        stfEncryptor.MessageEnd();
        return ciphertext;
    }
    else{
        ciphertext = text;
        CryptoPP::AES::Decryption aesDecryption(key, keyLength);
        CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption( aesDecryption, iv );
        CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink( plaintext ) );
        stfDecryptor.Put( reinterpret_cast<const unsigned char*>( ciphertext.c_str() ), ciphertext.size() );
        stfDecryptor.MessageEnd();
        return plaintext;
    }
    ///End of the cited code
    std::string result = "";
    if (encrypt){
        result = text;
    }
    else{
        for (unsigned int i = 0; i < text.size(); i++){
            char c = text.at(i);
            if ((int)c!=0){
                result += text.at(i);
            }
        }
    }
    return result;
}

/**
 * Finds the size of the file opened in the file stream
 * @param file - the file stream object
 * @return the size in bytes
 */
int Encryptor::findSize(std::fstream& file){
    if (file.is_open()){
        int beginning, ending;
        file.seekg(0, std::ios::beg);
        file.seekp(0, std::ios::beg);
        beginning = file.tellg();
        file.seekg(0, std::ios::end);
        ending = file.tellg();
        return ending - beginning;
    }
    return -1;
}

/**
 * Retrieves the key from key.bin to be used
 * to encrypt/decrypt the haystack
 * @param password - the user-entered password
 * @return the haystack key string
 */
std::string Encryptor::retrieveKey(std::string password){
    try{
        openKey(false);
    }
    catch(Encryptor::Status s){
        std::cout << "here9\n";
        throw s;
    }
    char keyArray[keyLength];
    for (unsigned int i=0; i < keyLength; i++){
        keyStream.seekg(i, std::ios::beg);
        char c;
        keyStream.get(c);
        keyArray[i] = (i>=password.size()) ?
        c : password.at(i);
    }
    for (unsigned int i = keyLength; i < password.size(); i++){
        int index = i % keyLength;
        int newValue =
            (((int)keyArray[index]) + ((int)password.at(i))) / 2;
        keyArray[index] = (char)newValue;
    }
    close(keyStream);
    std::string keyStr = "";
    for (unsigned int i =0; i < keyLength; i ++){
        keyStr += keyArray[i];
    }
    return keyStr;
}

/**
 * Encrypt the 3 haystack files(key[num].bin);
 * @param the user entered password string
 * @return the resulting status of the haystack encryption
 */
Encryptor::Status Encryptor::encryptHaystack(std::string password){
    std::string keyStr;
    try{
        keyStr = retrieveKey(password);
    }catch(Encryptor::Status s){
        std::cout << "here10\n";
        return s;
    }
    std::string ivStr = "";
    try{
        openKey(false);
    }
    catch(Encryptor::Status s){
        std::cout << "here11\n";
        return s;
    }
    for (int i = blockSize - 1; i >= 0; i--){
        char c;
        keyStream.seekg(i, std::ios::beg);
        keyStream.get(c);
        ivStr += c;
    }
    close(keyStream);
    Status stat;
    for (int i = 1; i < 4; i++){
        std::string num = Convert::intStr(i);
        std::string fileName =  directory + "key" + num + ".bin";
        stat = encryptFile(true, fileName, ivStr, keyStr);
        if (stat!=SUCCESS) return stat;
    }
    return stat;
}

/**
 * Decrypt the correct haystack file to be used to decrypt the main file
 * @param the user-entered password string
 * @return the resulting status of the haystack file decryption
 */
Encryptor::Status Encryptor::decryptHaystack(std::string password){
    try{
        openKey(false);
    }
    catch(Encryptor::Status s){
        std::cout << "here12\n";
        return s;
    }
    char info;
    keyStream.seekg(0, std::ios::beg);
    keyStream.get(info);
    info = Convert::intChar((int)info % 3 + 1);
    std::string fileName = directory + "key" + info + ".bin";
    std::string keyStr = retrieveKey(password);
    std::string ivStr = "";
    try{
        openKey(false);
    }
    catch(Encryptor::Status s){
        std::cout << "here13\n";
        return s;
    }
    for (int i = blockSize - 1; i >= 0; i--){
        char c;
        keyStream.seekg(i, std::ios::beg);
        keyStream.get(c);
        ivStr += c;
    }
    close(keyStream);
    return encryptFile(false, fileName, ivStr, keyStr);
}


/**
 * Encrypt/Decrypt a specified file with a given IV and key
 * @param encrypt - encrypt(true) or decrypt(false)
 * @param fileName - Full file location
 * @param ivStr - IV string
 * @param keyStr - Key string
 * @return the final enumerated result of the process
 */
Encryptor::Status Encryptor::encryptFile(bool encrypt, std::string fileName, std::string ivStr, std::string keyStr){
    std::string fileString = fileStr(fileName);
    std::string result;
    try{
        result = encryptText(encrypt, fileString, ivStr, keyStr);
    }
    catch(...){
        std::cout << "here14\n";
        if (!encrypt) return WRONG_PASSWORD;
        else return FAIL;
    }
    char fileArray[result.size()];
    for (unsigned int i = 0; i < result.size(); i++){
        fileArray[i] = result.at(i);
    }
    try{
        open(true, fileName);
    }
    catch(Encryptor::Status s){
        std::cout << "here15\n";
        return s;
    }
    mainStream.write(fileArray, result.size());
    close(mainStream);
    return SUCCESS;
}

/**
 * Perform the whole encryption process on a file
 * @param password - the password string
 * @param mainFile - the full file location
 * @return the enumerated result of the function
 */
Encryptor::Status Encryptor::encrypt(std::string password, std::string mainFile){
    Status stat;
    try{
        if (isEncrypted(mainFile) && isEncrypted()) return ENCRYPTED;
        if (password.size() > keySize1) keyLength = keySize2;
        if (password.size() > keySize2) keyLength = keySize3;
        generateHayStack();
        int fileNum = generateKeyFile();
        if (fileNum==-1) return KEY_NOT_FOUND;
        int pinVal = generatePIN(password);
        if (pinVal==-1) return KEY_NOT_FOUND;
        configureHaystack(pinVal, fileNum);
        std::string ivStr = generateIV(fileNum);
        std::string keyStr = generateKey(pinVal, fileNum);
        stat = encryptFile(true, mainFile, ivStr, keyStr);
        if (stat==SUCCESS) stat = encryptHaystack(password);
    }catch(Encryptor::Status s){
        std::cout << "here16\n";
        return s;
    }
    return stat;
}

/**
 * Perform the whole decryption process on a file
 * @param password - the password string
 * @param mainFile - the full file location
 * @return the enumerated result of the function
 */
Encryptor::Status Encryptor::decrypt(std::string password, std::string mainFile){
    Status stats;
    try{
        if (!isEncrypted(mainFile) && !isEncrypted()) return DECRYPTED;
        if (password.size() > keySize1) keyLength = keySize2;
        if (password.size() > keySize2) keyLength = keySize3;
        Status stats = decryptHaystack(password);
        if (stats!=SUCCESS) return stats;
        openKey(false);
        char info;
        keyStream.seekg(0, std::ios::beg);
        keyStream.get(info);
        close(keyStream);
        int fileNum = (int)info % 3 + 1;
        int PIN = generatePIN(password);
        std::string keyStr = generateKey(PIN, fileNum);
        std::string ivStr = generateIV(fileNum);
        stats = encryptFile(false, mainFile, ivStr, keyStr);
        if (stats!=SUCCESS){
            std::cout << "SUCC\n";
            std::string hayNum = directory + "key" + Convert::intStr(fileNum) + ".bin";
            keyStr = retrieveKey(password);
            char ivChar[keyStr.size()];
            for (unsigned int i = 0; i < keyStr.size(); i++){
                ivChar[keyStr.size()-1-i] = keyStr.at(i);
            }
            for (unsigned int i = 0; i < keyStr.size(); i++){
                if (i==0){
                    ivStr = ivChar[i];
                }
                else ivStr +=ivChar[i];
            }
            encryptFile(true, hayNum, ivStr, keyStr);
            return stats;
        }
         return stats;
    }catch(Encryptor::Status s){
        std::cout << "here17\n";
        return s;
    }
    return stats;
}



