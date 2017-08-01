#include "Encryptor.h"
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <iomanip>

#include "modes.h"
#include "aes.h"
#include "filters.h"
#include "Convert.h"

using namespace std;

/**
 * Default constructor
 */
Encryptor::Encryptor(){
    directory = "";
}

/**
 * Constructor with key directory passed
 */
Encryptor::Encryptor(string keyDir){
    directory = keyDir;
};

/**
 * Opens key.bin
 * @param write - true if open for write-only, false if open for reading
 */
bool Encryptor::openKey(bool write){
    string keyLocation = directory + "key.bin";
    if (keyStream.is_open()) keyStream.close();
    if (write){
        keyStream.open(keyLocation.c_str(), ios::out|ios::binary|ios::trunc);
    }
    else{
        keyStream.open(keyLocation.c_str(), ios::in|ios::binary|ios::ate);
    }
    return (keyStream.is_open());
}

/**
 * Opens a specified file for write or read mode
 * @param write - write(true) or read(false) mode
 * @param fileName - full file name (directory, file, and extension)
 */
void Encryptor::open(bool write, string fileName){
    if (mainStream.is_open()) mainStream.close();
    if (write){
        mainStream.open(fileName.c_str(), ios::out|ios::binary|ios::trunc);
    }
    else{
        mainStream.open(fileName.c_str(), ios::in|ios::binary|ios::ate);
    }
}

/**
 * Opens a file for both write and read operation
 * @param fileName - full file name
 */
void Encryptor::open(string fileName){
    if (mainStream.is_open()) mainStream.close();
    mainStream.open(fileName.c_str(), ios::out|ios::binary|ios::in);
}

/**
 * Closes a specified file stream if it is open
 * @stream the file stream object
 */
void Encryptor::close(fstream& stream){
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
        string fileName =  directory + "key" + Convert::intStr(i) + ".bin";
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
    keyStream.seekp(0, ios::beg);
    keyStream.write(primaryKey, 16);
    close(keyStream);
    return random;
}

/**
 * Return the content of the file
 * in the form of a string object
 * @param fileName - full file name
 * @return the file's content in string
 */
string Encryptor::fileStr(string fileName){
    string fileString = "null";
    char stringCreator;                             //Character variable to add on to the string
    open(false, fileName);
    int fileSize = findSize(mainStream);  //Determine file size
    for (int i = 0; i < fileSize; i++){
        mainStream.seekg(i);                         //put getter at the index
        mainStream.get(stringCreator);               //assign the content to the character
            //if (stringCreator=='\n') i++;
        if (fileString=="null") {
            fileString = stringCreator;             //If not created yet, the first character is assigned to the string
            continue;                               //Makes sure that it does not go on to adding the first character to the string twice
        }
        fileString += stringCreator;                //Add on to the string
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
    string num = Convert::intStr(fileNum);
    string hayNum = directory + "key" + num + ".bin";
    open(hayNum);
    srand(time(0));
    int ivNum = 0;
    int place = 1000;
    for (int i=10016; i < 10024; i+=2){
        int random = rand()%10;
        ivNum += (random * place);
        place /= 10;
        mainStream.seekp(i, ios::beg);
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
        mainStream.seekp(i, ios::beg);
        mainStream.put(num);
    }
    close(mainStream);
}

/**
 * Generates the IV by combining the 1st, 3rd, 5th, and 7th digits
 * to specify the starting character index.  Then 16 subsequent characters
 * starting from that index are read to generate a 16-char IV
 * @param fileNum the number of the key[num].bin file
 * @return the IV string to be used for Enc/Dec
 */
string Encryptor::generateIV(int fileNum){
    string ivStr;
    string num = Convert::intStr(fileNum);
    string keyFile = directory + "key" + num + ".bin";
    open(false, keyFile);
    for (int i = 10016; i < 10024; i+=2){
        char digit;
        mainStream.seekg(i, ios::beg);          //move cursor to the beginning of the file where passcode is
        mainStream.get(digit);
        if (i==0) ivStr=digit;
        else ivStr+=digit;
    }
    int ivPin = Convert::strInt(ivStr);
    mainStream.seekg(ivPin, ios::beg);
    char finalIV[16];
    mainStream.read(finalIV, 16);
    close(mainStream);
    return Convert::arrayStr(finalIV, sizeof(finalIV));
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
string Encryptor::generateKey(bool usingPIN, int PIN, int fileNum){
    string num = Convert::intStr(fileNum);
    string keyFile = directory + "key" + num + ".bin";
    open(false, keyFile);
    int ivPIN, arbPIN, indexPIN;
    if (!usingPIN){
        string ivStr;
        for (int i = 10016; i < 10024; i+=2){
            char digit;
            mainStream.seekg(i, ios::beg);          //move cursor to the beginning of the file where passcode is
            mainStream.get(digit);
            if (i==10016) ivPIN=digit;
            else ivStr+=digit;
        }
        ivPIN = Convert::strInt(ivStr);
    }
    string pinStr;
    for (int i = 10017; i < 10024; i+=2){
        char num[1];
        mainStream.seekg(i, ios::beg);          //move cursor to the beginning of the file where passcode is
        mainStream.read(num, 1);
        if (i==10017) pinStr=num[0];
        else pinStr+=num[0];
    }
    arbPIN = Convert::strInt(pinStr);
    indexPIN = usingPIN ? Convert::bitwiseXOR(PIN, arbPIN) :
        Convert::bitwiseXOR(ivPIN, arbPIN);
    char draftKey[4][4];
    int place = 1000;
    for (int i=0; i<4; i++){
        mainStream.seekg(indexPIN+Convert::digit(arbPIN, place), ios::beg);
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
    return Convert::arrayStr(finalKey, sizeof(finalKey));
}

/**
 * Checks if the haystack files themselves are all encrypted or not
 * by checking if each character's integer value is between 0 and 127
 * @return whether all 3 files are encyrpted or not
 */
bool Encryptor::isEncrypted(){
    bool encrypted[3];
    for (int i = 0; i < 3; i++){
        string fileName = directory + "key" + Convert::intStr(i+1) + ".bin";
        string dataFeed = fileStr(fileName);
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
bool Encryptor::isEncrypted(string fileName){
    open(false, fileName);
    string dataFeed = fileStr(fileName);
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
 * @return the resulting string from encryption/decryption
 */
string Encryptor::encryptText(bool encrypt, string text, string ivStr, string keyStr){
    string plaintext, ciphertext;
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
    string result = "";
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
int Encryptor::findSize(fstream& file){
    if (file.is_open()){
        int beginning, ending;
        file.seekg(0, ios::beg);     //Put file character getter in the beginning
        file.seekp(0, ios::beg);
        beginning = file.tellg();    //Assign the beginning index
        file.seekg(0, ios::end);     //Put getter in the end
        ending = file.tellg();       //Assign the end index
        return ending - beginning;      //Difference in the two numbers determines size in bytes
    }
    return -1;
}

/**
 * Retrieves the 16-char key from key.bin to be used
 * to encrypt/decrypt the haystack
 *
 */
std::string Encryptor::retrieveKey(bool encrypting, bool usedPIN, string password){
    openKey(false);
    char keyArray[16];
    string keyStr;
    char info;
    bool PIN = true;
    if (!encrypting){
        keyStream.seekg(0, ios::beg);
        keyStream.get(info);
        PIN  = (info=='T');
    }
    else PIN = usedPIN;
    if (PIN){
        keyStream.seekg(0, ios::beg);
        keyStream.read(keyArray, 16);
    }
    else{
        for (unsigned int i=0; i < 16; i++){
            keyStream.seekg(i, ios::beg);
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
Encryptor::Status Encryptor::encryptHaystack(bool usingPIN, string password){
    string keyStr = retrieveKey(true, usingPIN, password);
    string ivStr = "abcdefghijklmnop";
    for (unsigned int i = 0; i < keyStr.size(); i++){
        ivStr.at(keyStr.size()-1-i) = keyStr.at(i);
    }
    Status stat;
    for (int i = 1; i < 4; i++){
        string num = Convert::intStr(i);
        string fileName =  directory + "key" + num + ".bin";
        stat = encryptFile(true, fileName, ivStr, keyStr);
        if (stat!=SUCCESS) return stat;
    }
    return stat;
}

/**
 * Decrypt the correct haystack file to be used to decrypt the main file
 */
Encryptor::Status Encryptor::decryptHaystack(string password){
    if (!openKey(false)) return KEY_NOT_FOUND;
    char info[2];
    keyStream.seekg(0, ios::beg);
    keyStream.read(info, 2);
    string num = Convert::charStr(info[1]);
    string fileName = directory + "key" + num + ".bin";
    string keyStr = retrieveKey(false, false, password);
    string ivStr;
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
///TODO: Continue documentation here tomorrow
/**
 *
 */
Encryptor::Status Encryptor::encryptFile(bool encrypt, string fileName, string ivStr, string keyStr){
    string fileString = fileStr(fileName);
    string result;
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


Encryptor::Status Encryptor::encrypt(bool usingPIN, string password, string mainFile){
    Status stat;
    if (isEncrypted(mainFile) && isEncrypted()) return ENCRYPTED;
    generateHayStack();
    int fileNum = generateKeyFile(usingPIN);
    if (fileNum==-1) return KEY_NOT_FOUND;
    srand(time(0));
    int pinVal = usingPIN ?
        Convert::strInt(password) :
        (rand() % 10000);
    if (pinVal < 0) pinVal *= -1;
    configureHaystack(usingPIN, pinVal, fileNum);
    string ivStr = generateIV(fileNum);
    string keyStr = generateKey(usingPIN, pinVal, fileNum);
    stat = encryptFile(true, mainFile, ivStr, keyStr);
    if (stat==SUCCESS) stat = encryptHaystack(usingPIN, password);
    return stat;
}

Encryptor::Status Encryptor::decrypt(string password, string mainFile){
    if (!isEncrypted(mainFile) && !isEncrypted()) return DECRYPTED;
    Status stats = decryptHaystack(password);
    if (stats!=SUCCESS) return stats;
    openKey(false);
    char info[2];
    keyStream.seekg(0, ios::beg);
    keyStream.read(info, 2);
    close(keyStream);
    bool usedPin = (info[0]=='T');
    int pin = 1111;
    if (usedPin){
        pin = Convert::strInt(password);
    }
    int fileNum = Convert::charInt(info[1]);
    string keyStr = generateKey(usedPin, pin, fileNum);
    string ivStr = generateIV(fileNum);
    stats = encryptFile(false, mainFile, ivStr, keyStr);
    if (stats!=SUCCESS){
        string hayNum = directory + "key" + Convert::intStr(fileNum) + ".bin";
        openKey(false);
        char info[2];
        keyStream.seekg(0, ios::beg);
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



