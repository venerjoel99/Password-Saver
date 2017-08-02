#include "Encryptor.h"
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <iomanip>

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

/**
 * "Generate the haystack" by creating 3 key[num].bin
 *  files.  Each file is created by inserting 10016
 *  randomly generated characters and 8 random numerical
 *  digits at the end, resulting each file size of 10024 characters each
 */
void Encryptor::generateHayStack(){
    srand(time(0));
    for (int i = 1; i < 4; i++){
        std::string fileName =  directory + "key" + Convert::intStr(i) + ".bin";
        open(true, fileName);
        char passcode[10024];
        for (int i=0; i < 10016; i++){
            int random = rand()%93;
            passcode[i] = '!' + random;
        }
        for (int i=10016; i < 10024; i++){
            int random = rand()%10;
            char num = Convert::intChar(random);
            passcode[i] = num;
        }
        mainStream.write(passcode, 10024);
        close(mainStream);
    }
}

/**
 * A 16 character binary file(key.bin) is created that
 * would be used to encrypt or decrypt the "haystack"
 * and obtain information as to whether how the encryption is done
 * The first character is assigned 'T' if a PIN was used and 'F' if
 * a password was used for encryption.  The second character is assigned
 * a random number between 1 and 3, which specifies which key[num].bin file
 * to truly configure for encryption and use for decryption
 * @param usingPIN - Did the user select the PIN method
 */
int Encryptor::generateKeyFile(bool usingPIN){
    char primaryKey[16];
    ///
    primaryKey[0] = usingPIN ? 'T' : 'F';
    srand(time(0));
    int random = (rand() % 3) + 1;
    primaryKey[1] = Convert::intChar(random);
    for (int i = 2; i < 16; i++){
        int random = rand()%93;
        primaryKey[i] = '!' + random;
    }
    if (!openKey(true)) return -1;
    keyStream.seekp(0, std::ios::beg);
    keyStream.write(primaryKey, 16);
    close(keyStream);
    return random;
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
 * Configures the haystack by changing the specified key[num].bin file's
 * digit to be configured for encryption.
 * The 8 digits are configured by:
 *  -Assigning the 1st, 3rd, 5th, and 7th digits
 *   random digits to be used as a four-digit index of the IV in the key[num].bin
 *  -Assigning the 2nd, 4th, 6th, and 8th digits
 *   digits obtained from XORing the digits of the PIN and a randomly generated number(0-1000) if PIN is used
 *   or from XORing the digits of the IV index and the random number if a password is used.
 *
 */
void Encryptor::configureHaystack(bool usingPIN, int PIN, int fileNum){
    std::string num = Convert::intStr(fileNum);
    std::string hayNum = directory + "key" + num + ".bin";
    open(hayNum);
    srand(time(0));
    int ivNum = 0;
    int place = 1000;
    for (int i=10016; i < 10024; i+=2){
        int random = rand()%10;
        ivNum += (random * place);
        place /= 10;
        mainStream.seekp(i, std::ios::beg);
        mainStream.put(Convert::intChar(random));
    }
    int randPIN = rand()%10000;
    int arbNum = usingPIN ? Convert::bitwiseXOR(PIN, randPIN) :
        Convert::bitwiseXOR(ivNum, randPIN);
    place = 1000;
    for (int i=10017; i < 10024; i+=2){
        char num;
        num = Convert::intChar(Convert::digit(arbNum, place));
        place /= 10;
        mainStream.seekp(i, std::ios::beg);
        mainStream.put(num);
    }
    close(mainStream);
}

/**
 * Generates the IV by combining the 1st, 3rd, 5th, and 7th digits
 * to specify the starting character index.  Then 16 subsequent characters
 * starting from that index are read to generate a 16-char IV
 * @param fileNum the number of the key[num].bin file
 * @return the IV std::string to be used for Enc/Dec
 */
std::string Encryptor::generateIV(int fileNum){
    std::string ivStr;
    std::string num = Convert::intStr(fileNum);
    std::string keyFile = directory + "key" + num + ".bin";
    open(false, keyFile);
    for (int i = 10016; i < 10024; i+=2){
        char digit;
        mainStream.seekg(i, std::ios::beg);
        mainStream.get(digit);
        if (i==0) ivStr=digit;
        else ivStr+=digit;
    }
    int ivPin = atoi(ivStr.c_str());
    mainStream.seekg(ivPin, std::ios::beg);
    char finalIV[16];
    mainStream.read(finalIV, 16);
    close(mainStream);
    return std::string(finalIV);
}

/**
 * Generates the key by:
 *   - XORing the PIN digits(PIN method) or IV index(password method) with the
 *     arbitrary digits(2nd, 4th, 6th, and 8th) to generate the Key index
 *   - The arbitrary digits are then used to specify which 4 blocks of 4 characters to
 *     use to make the 16-char key
 *     Exmaple:
 *         - Key: 1324, arbDigits: 1, 4, 5, 2
 *         - key = block of 4[index: 1324 + 1]
 *                  + block of 4[index: 1324 + 4]
 *                  + block of 4[index: 1324 + 5]
 *                  + block of 4[index: 1324 + 2]
 * @param usingPIN - whether the user uses the PIN or password
 * @param PIN - the four digit PIN(if usingPIN is false, any number can be put in)
 * @param fileNum - the number of the key[num].bin file
 * @return the generated 16 char key
 */
std::string Encryptor::generateKey(bool usingPIN, int PIN, int fileNum){
    std::string num = Convert::intStr(fileNum);
    std::string keyFile = directory + "key" + num + ".bin";
    open(false, keyFile);
    int ivPIN, arbPIN, indexPIN;
    if (!usingPIN){
        std::string ivStr;
        for (int i = 10016; i < 10024; i+=2){
            char digit;
            mainStream.seekg(i, std::ios::beg);
            mainStream.get(digit);
            if (i==10016) ivPIN=digit;
            else ivStr+=digit;
        }
        ivPIN = atoi(ivStr.c_str());
    }
    std::string pinStr;
    for (int i = 10017; i < 10024; i+=2){
        char num[1];
        mainStream.seekg(i, std::ios::beg);
        mainStream.read(num, 1);
        if (i==10017) pinStr=num[0];
        else pinStr+=num[0];
    }
    arbPIN = atoi(pinStr.c_str());
    indexPIN = usingPIN ? Convert::bitwiseXOR(PIN, arbPIN) :
        Convert::bitwiseXOR(ivPIN, arbPIN);
    char draftKey[4][4];
    int place = 1000;
    for (int i=0; i<4; i++){
        mainStream.seekg(indexPIN+Convert::digit(arbPIN, place), std::ios::beg);
        mainStream.read(draftKey[i], 4);
        place /= 10;
    }
    char finalKey[16];
    for (int i=0; i<4; i++){
        for (int j=0; j<4; j++){
            finalKey[(4*i)+j] = draftKey[i][j];
        }
    }
    close(mainStream);
    return std::string(finalKey);
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
            if ((int)charChecker[k]<0 ||
                (int)charChecker[k]>127){
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
        if ((int)charChecker[i]<0 ||
            (int)charChecker[i]>128){
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
    byte key[ CryptoPP::AES::DEFAULT_KEYLENGTH ], iv[ CryptoPP::AES::BLOCKSIZE ];
    memmove(key, keyArray,  CryptoPP::AES::DEFAULT_KEYLENGTH);
    memmove(iv, ivArray, CryptoPP::AES::BLOCKSIZE);
    ///Source of the following code: https://stackoverflow.com/questions/12306956/example-of-aes-using-crypto
    ///Begin cited code
    if (encrypt){
        plaintext = text;
        CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
        CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );
        CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink( ciphertext ) );
        stfEncryptor.Put( reinterpret_cast<const unsigned char*>( plaintext.c_str() ), plaintext.length() + 1 );
        stfEncryptor.MessageEnd();
        return ciphertext;
    }
    else{
        ciphertext = text;
        CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
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
std::string Encryptor::retrieveKey(bool encrypting, bool usedPIN, std::string password){
    openKey(false);
    char keyArray[16];
    std::string keyStr;
    char info;
    bool PIN = true;
    if (!encrypting){
        keyStream.seekg(0, std::ios::beg);
        keyStream.get(info);
        PIN  = (info=='T');
    }
    else PIN = usedPIN;
    if (PIN){
        keyStream.seekg(0, std::ios::beg);
        keyStream.read(keyArray, 16);
    }
    else{
        for (unsigned int i=0; i < 16; i++){
            keyStream.seekg(i, std::ios::beg);
            char c;
            keyStream.get(c);
            if (i>=password.size()) keyArray[i] = c;
            else keyArray[i] = password.at(i);
        }
    }
    for (int i = 0; i < 16; i++){
        if (i==0){
            keyStr = keyArray[i];
        }
        else{
            keyStr+= keyArray[i];
        }
    }
    close(keyStream);
    return keyStr;
}

/**
 * Encrypt the 3 haystack files(key[num].bin);
 */
Encryptor::Status Encryptor::encryptHaystack(bool usingPIN, std::string password){
    std::string keyStr = retrieveKey(true, usingPIN, password);
    std::string ivStr = "abcdefghijklmnop";
    for (unsigned int i = 0; i < keyStr.size(); i++){
        ivStr.at(keyStr.size()-1-i) = keyStr.at(i);
    }
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
    keyStream.seekg(1, std::ios::beg);
    keyStream.get(info);
    std::string fileName = directory + "key" + info + ".bin";
    std::string keyStr = retrieveKey(false, false, password);
    std::string ivStr;
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
Encryptor::Status Encryptor::encrypt(bool usingPIN, std::string password, std::string mainFile){
    Status stat;
    if (isEncrypted(mainFile) && isEncrypted()) return ENCRYPTED;
    generateHayStack();
    int fileNum = generateKeyFile(usingPIN);
    if (fileNum==-1) return KEY_NOT_FOUND;
    srand(time(0));
    int pinVal = usingPIN ?
        atoi(password.c_str()) :
        (rand() % 10000);
    if (pinVal < 0) pinVal *= -1;
    configureHaystack(usingPIN, pinVal, fileNum);
    std::string ivStr = generateIV(fileNum);
    std::string keyStr = generateKey(usingPIN, pinVal, fileNum);
    stat = encryptFile(true, mainFile, ivStr, keyStr);
    if (stat==SUCCESS) stat = encryptHaystack(usingPIN, password);
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
    Status stats = decryptHaystack(password);
    if (stats!=SUCCESS) return stats;
    openKey(false);
    char info[2];
    keyStream.seekg(0, std::ios::beg);
    keyStream.read(info, 2);
    close(keyStream);
    bool usedPin = (info[0]=='T');
    int pin = 1111;
    if (usedPin){
        pin = atoi(password.c_str());
    }
    int fileNum = Convert::charInt(info[1]);
    std::string keyStr = generateKey(usedPin, pin, fileNum);
    std::string ivStr = generateIV(fileNum);
    stats = encryptFile(false, mainFile, ivStr, keyStr);
    if (stats!=SUCCESS){
        std::string hayNum = directory + "key" + Convert::intStr(fileNum) + ".bin";
        openKey(false);
        char info[2];
        keyStream.seekg(0, std::ios::beg);
        keyStream.read(info, 2);
        close(keyStream);
        keyStr = retrieveKey(true, usedPin, password);
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



