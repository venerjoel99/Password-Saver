#include "Encryptor.h"
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <cmath>

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
bool Encryptor::openKey(bool write){
    std::string keyLocation = directory + "key.bin";
    if (keyStream.is_open()) keyStream.close();
    if (write){
        keyStream.open(keyLocation.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
    }
    else{
        keyStream.open(keyLocation.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
    }
    return (keyStream.is_open());
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
}

/**
 * Opens a file for both write and read operation
 * @param fileName - full file name
 */
void Encryptor::open(std::string fileName){
    if (mainStream.is_open()) mainStream.close();
    mainStream.open(fileName.c_str(), std::ios::out|std::ios::binary|std::ios::in);
}

/**
 * Closes a specified file stream if it is open
 * @stream the file stream object
 */
void Encryptor::close(std::fstream& stream){
    if (stream.is_open()) stream.close();
}

int Encryptor::generatePIN(std::string password){
    if (!openKey(false)) return -1;
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
        std::string fileName =  directory + "key" + Convert::intStr(i) + ".bin";
        open(true, fileName);
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
 * @param usingPIN - Did the user select the PIN method
 */
int Encryptor::generateKeyFile(){
    char primaryKey[keyLength];
    srand(time(0));
    for (int i = 0; i < keyLength; i++){
        int random = rand()%93;
        primaryKey[i] = '!' + random;
    }
    if (!openKey(true)) return -1;
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
    open(false, fileName);
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
 * for one time encryption and decryption
 * @param usingPIN - whether the user used a PIN for encryption
 * @param PIN - user-entered PIN or any number(if no PIN entered)
 * @param fileNum - the number of the key[num].bin file to be used
 */
void Encryptor::configureHaystack(int PIN, int fileNum){
    std::string num = Convert::intStr(fileNum);
    std::string hayNum = directory + "key" + num + ".bin";
    open(hayNum);
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
    std::string num = Convert::intStr(fileNum);
    std::string keyFile = directory + "key" + num + ".bin";
    open(false, keyFile);
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
 *
 * @param usingPIN - whether the user uses the PIN or password
 * @param PIN - the four digit PIN(if usingPIN is false, any number can be put in)
 * @param fileNum - the number of the key[num].bin file
 * @return the generated 16 char key
 */
std::string Encryptor::generateKey(int PIN, int fileNum){
    std::string num = Convert::intStr(fileNum);
    std::string keyFile = directory + "key" + num + ".bin";
    open(false, keyFile);
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
        std::string dataFeed = fileStr(fileName);
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
    open(false, fileName);
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
 * @return the resulting std::string from encryption/decryption
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
 *
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
 * Retrieves the 16-char key from key.bin to be used
 * to encrypt/decrypt the haystack
 *
 */
std::string Encryptor::retrieveKey(std::string password){
    openKey(false);
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
 */
Encryptor::Status Encryptor::encryptHaystack(std::string password){
    if (!openKey(false)) return KEY_NOT_FOUND;
    std::string keyStr = retrieveKey(password);
    std::string ivStr = "";
    if (!openKey(false)) return KEY_NOT_FOUND;
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
 */
Encryptor::Status Encryptor::decryptHaystack(std::string password){
    if (!openKey(false)) return KEY_NOT_FOUND;
    char info;
    keyStream.seekg(0, std::ios::beg);
    keyStream.get(info);
    info = Convert::intChar((int)info % 3 + 1);
    std::string fileName = directory + "key" + info + ".bin";
    std::string keyStr = retrieveKey(password);
    std::string ivStr = "";
    if (!openKey(false)) return KEY_NOT_FOUND;
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
        if (!encrypt) return WRONG_PASSWORD;
        else return FAIL;
    }
    char fileArray[result.size()];
    for (unsigned int i = 0; i < result.size(); i++){
        fileArray[i] = result.at(i);
    }
    open(true, fileName);
    mainStream.write(fileArray, result.size());
    close(mainStream);
    return SUCCESS;
}

/**
 * Perform the whole encryption process on a file
 * @param usingPIN - PIN(true) or password(false)
 * @param password - the PIN or password string
 * @param mainFile - the full file location
 * @return the enumerated result of the function
 */
Encryptor::Status Encryptor::encrypt(std::string password, std::string mainFile){
    Status stat;
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
    return stat;
}

/**
 * Perform the whole decryption process on a file
 * @param password - the PIN or password string
 * @param mainFile - the full file location
 * @return the enumerated result of the function
 */
Encryptor::Status Encryptor::decrypt(std::string password, std::string mainFile){
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
}



