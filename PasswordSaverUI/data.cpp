#include <fstream>
#include <string>
#include <iostream>
#include "data.h"
#include "Encryptor.h"

using namespace std;

///Constructor for Info class
Info::Info(string website, string username, string password){
    this->website = website;
    this->username = username;
    this->password = password;
}

/**
 * Static function for prompting user input
 * @return The user's input's string
 */
string Info::input(){
    string info;
    cin.sync();
    getline(cin, info);
    return info;
}

/**
 * Converts info object into string object
 * @return Converted info string
 */
string Info::toStr(){
    string infoString =
    "Website: " + website + "\n"
    + "Username: " + username + "\n"
    + "Password: " + password + "\n";
    return infoString;
}

///Writes objects into text files
fstream& operator<<(fstream& streaming, Info info){
    if (streaming.is_open()){
        streaming << info.getWebsite() << std::endl;
        streaming << info.getUsername() << std::endl;
        streaming << info.getPassword() << std::endl << std::endl;
    }
    else return streaming;
    return streaming;
}

/**
 * Finds the size of the file in the file stream object
 * @return File size in bytes
 */
int findSize(fstream& file){
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

///Inputs text file content into string
fstream& operator>>(fstream& stream, string& fileString){
    if (stream.is_open()){
        fileString = "null";
        char stringCreator;                             //Character variable to add on to the string
        int fileSize = findSize(stream);  //Determine file size
        for (int i = 0; i < fileSize; i++){
            stream.seekg(i);                         //put getter at the index
            stream.get(stringCreator);               //assign the content to the character
            //if (stringCreator=='\n') i++;
            if ((int)stringCreator==0) {
                continue;
            }
            if (fileString=="null") {
                fileString = stringCreator;             //If not created yet, the first character is assigned to the string
                                                //Makes sure that it does not go on to adding the first character to the string twice
            }
            else fileString += stringCreator;                //Add on to the string
        }
    }
    else return stream;
    return stream;
}

/**
 * Constructor for Data class
 * @param file - the file name and extension
 */
Data::Data(std::string file){
    fileDir = "";
    fileName = file;
    encrypt(true);
}

/**
 * Default constructor for Data class
 */
Data::Data(){
    encrypt(true);
}

/**
 * Modified destructor for the Data class
 */
Data::~Data(){
    if (theFile.is_open()) theFile.close();
    encrypt(true);
}

/**
 * Closes the file if it is open
 * Reopens the file in the specified open mode
 * @param mode - how the file opened is going to be used
 * @return whether the file is open
 */
bool Data::open(OpenMode mode){
    if (currentMode==mode && isOpen()) return theFile.is_open();
    close();
    string fullFile = fileDir + fileName;
    switch(mode){
        case REGULAR:
            theFile.open(fullFile.c_str(), ios::out | ios::in | ios::app | ios::binary);
            break;
        case ENCRYPTION:
            theFile.open(fullFile.c_str(), ios::out | ios::in | ios::binary);
            break;
        case REWRITE:
            theFile.open(fullFile.c_str(), ios::out|ios::binary|ios::trunc);
            break;
        case READ_ONLY:
            theFile.open(fullFile.c_str(), ios::in|ios::binary|ios::ate);
            break;
        default:
            theFile.open(fullFile.c_str(), ios::out | ios::in | ios::app | ios::binary);
    }
    currentMode = mode;
    return theFile.is_open();
}

/**
 * Closes the file if it is open
 */
void Data::close(){
    if (isOpen()){
        currentMode=CLOSED;
        theFile.close();
    }
}

/**
 * Searches the file for the index of the keyword
 * @param keyword - the word to be searched in the file
 * @return the index of the first character of the word
 * or -1 if no result is found
 */
int Data::searchEngine(string keyword){
    if (!open(REGULAR)) return -1;
    string result;
    int index = 0;
    //Let user type the search word
    //Find the size of the file
    int fileSize = findSize();
    //Find the size of the keyword
    int length = keyword.size();
    //Variables for the for loop
    char compared, character, newLine;
    for(int i = 0; i<fileSize; i++){
        //Set the first letter of keyword as comparison
        compared = keyword.at(index);
        //If it reaches the end, let user know there is no results for the keyword
        if (i==fileSize-1){
            //cout << "No results found\n";
            return -1;
        }
        theFile.seekg(i, ios::beg);       //Put getter at the index
        theFile.get(character);           //Assign the variable the character
        if (character==compared && i > 0){
            theFile.seekg(i-1, ios::beg);
            theFile.get(newLine);
            if ((int)newLine!=0 && newLine!='\n') continue;
        }
        //If the letter at that index matches the input's character, this loop will keep adding characters until
        //the inputted string and the created string match which confirms the location of the keyword
        while ((compared==character)&&(index<length)){
            compared = keyword.at(index);   //Assign letter of the inputted keyword into the input character variable
            theFile.seekg(i + index);     //Put getter a certain number away from the first match of characters
            theFile.get(character);       //Assign letter of the file content into the file character variable
            if (index==0){result = character;}  //If it's the first match, create the array of characters
            else {result += character;}         //If not, add on to the array
            index++;
        }
        //If the keyword user entered matches the result created by the program,
        //return the starting location of the keyword in the file
        if (keyword==result){
            return i;
        }
        index=0;
    }
    return -1;
}

/**
 * Reads 3 lines of the file from a starting index
 * to generate a data structure of website, username, and password
 * @param position - the starting index to read characters from
 * @return an Info data structure object containing website, username, and password
 * or contains "Null" if no result
 */
Info Data::read(int position){
    //Info result;    //class object to be returned
    //If file is open and index is not negative
    string website, username, password;
    if (open(REGULAR) && position >= 0){
        int enterCount = 0; //Determines if it adds to credential, username, or password
        int consecutiveEnters = 0;  //If it equals two, then username or password was entered
        char indicator; //character variable that adds into string variables
        //set strings to null for first characters to be added
        website = username = password = "null";
        position--; //decrease by 1 before starting the loop so loop starts at 0.
        while (enterCount < 3){
            position++;
            theFile.seekg(position);
            theFile.get(indicator);   //place getter and assign the character there
            if (indicator=='\n'){
                enterCount++;   //if newline is reached, then add 1 to change string variable to be added on
                consecutiveEnters++;    //increment the consecutive enters to keep track of error
                //if the search function returned the keyword index but the 2 consecutive enters have been reached,
                //this means that the username or password was entered
                if (consecutiveEnters > 1){
                    //Give user nulls for being stupid not to follow directions
                    Info nullResult("Null", "No", "Result");
                    return nullResult;
                }
            }
            else {
                //If not newline, set this to zero to every time to rid of the streak
                consecutiveEnters = 0;
                //Add character to credential string if zero newlines have been passed through
                if (enterCount==0){
                    if (website=="null"){
                        website = indicator;
                    }
                    else website +=indicator;
                }
                //Add to username if newline has been reached once
                else if(enterCount==1){
                    if (username=="null"){
                        username = indicator;
                    }
                    else username +=indicator;
                }
                //Add to password if newline has been reached twice
                else if(enterCount==2){
                    if (password=="null"){
                        password = indicator;
                    }
                    else password +=indicator;
                }
            }
        }
    }
    else {
        Info nullInfo("Null", "No", "Result");
        //cout << "File not found\n";
        return nullInfo;
    }
    Info result(website, username, password);
    return result;
}

/**
 * Checks if a file is opened
 * @return whether file is opened and the OpenMode
 * enum is not closed
 */
bool Data::isOpen(){
    return currentMode!=CLOSED && theFile.is_open();
}

/**
 * Finds the size of the file in
 * the file stream member
 * @return the size in number of characters in the file
 */
int Data::findSize(){
    if (open(REGULAR)){
        int beginning, ending;
        theFile.seekg(0, ios::beg);     //Put file character getter in the beginning
        beginning = theFile.tellg();    //Assign the beginning index
        theFile.seekg(0, ios::end);     //Put getter in the end
        ending = theFile.tellg();       //Assign the end index
        return ending - beginning;      //Difference in the two numbers determines size in bytes
    }
    return -1;
}

/**
 * Public function to return an Info data structure
 * from inserting a search keyword
 * @param searchWord - the keyword to search in the file
 * @return the Info data structure result if any
 */
Info Data::findSearch(string searchWord){
    return read(searchEngine(searchWord));
}

/**
 * Write an Info data structure into the file
 * @param info - the data structure
 * @return the resulting status of the function
 */
Data::Success Data::writeIntoFile(Info info){
    if(open(REGULAR)){
        theFile << info;
        return SUCCESS;
    }
    return FILE_NOT_FOUND;
}

/**
 * Read the file content into a string object
 * @param dataString - the string object to insert file content into
 * @return the resulting status of the function
 */
Data::Success Data::readFromFile(string& dataString){
    if(open(REGULAR)){
        theFile >> dataString;
        return SUCCESS;
    }
    return FILE_NOT_FOUND;
}

/**
 * Changes the file location the file stream opens
 * @param newDir - the file's folder location
 * @param newFile - the file name and extension
 * @return the resulting enumerated status of the function
 */
Data::Success Data::changeFile(std::string newDir, std::string newFile){
    fileDir = newDir;
    fileName = newFile;
    if (!open(REGULAR)) return FILE_NOT_FOUND;
    return SUCCESS;
}

/**
 * Changes the file location without directory
 * @param newFile - the file name and extension
 * @param clearDir - whether to open the file in the same directory
 *        or open the file with no directory at all
 * @return the resulting status of the function
 */
Data::Success Data::changeFile(std::string newFile, bool clearDir){
    return clearDir ?
        changeFile("", newFile) :
        changeFile(fileDir, newFile);
}

/**
 * Change the info of one of the entries in the file
 * @param website - the specified website entry to change
 * @param info - the new information string
 * @param flag - US(change username) or PS(change password)
 * @return the result of the function
 */
Data::Success Data::changeInfo(string website, string info, Change flag){
    string fileString;
    Success result = readFromFile(fileString);
    if (result!=SUCCESS){
        return result;
    }
    int position = -1;
    int enterCount = 0;
    char indicator;
    position = searchEngine(website);
    if (position==-1) return NO_RESULTS;
    string sub1 = fileString.substr(0,position);
    Info old = read(position);
    while (enterCount<4){
        position++;
        theFile.seekg(position);
        theFile.get(indicator);
        enterCount += indicator=='\n' ? 1 : 0;
    }
    position++;
    string sub2 = fileString.substr(position, fileString.size()-position);
    Info newInfo(old.getWebsite(),
                 (flag==US) ? info : old.getUsername(),
                 (flag==PS) ? info : old.getPassword());
    string newFileString;
    if (flag!=DE){
        newFileString = sub1 + newInfo.getWebsite()
        + "\n" + newInfo.getUsername() + "\n"
        + newInfo.getPassword() + "\n\n"
        + sub2;
    }
    else{
        newFileString = sub1 + sub2;
    }
    if (!open(REWRITE)) return FILE_NOT_FOUND;
    char fileArray[newFileString.size()];
    for (unsigned int i = 0; i < newFileString.size(); i++){
        fileArray[i] = newFileString.at(i);
    }
    theFile.write(fileArray, newFileString.size());
    return SUCCESS;
}

/**
 * Change the username or password for an entry
 * in the file
 * @param website - the website entry to edit
 * @param newUsername - the new user name
 * @param newPassword - the new password
 * @return the enumerated result of the function
 */
Data::Success Data::changeInfo(string website, string newUsername, string newPassword){
    Success stat = changeInfo(website, newUsername, US);
    if (stat!=SUCCESS) return stat;
    return changeInfo(website, newPassword, PS);
}

/**
 * Encrypt the file with either PIN or passwowrd
 * @param usePIN - whether to use PIN or password
 * @param passcode - the PIN or password string
 * @return the Encryptor class generated enumeration result
 */
Encryptor::Status Data::encrypt(bool usePin, string passcode){
    int periodPos = fileName.find('.');
    std::string keyFolder = periodPos!=std::string::npos ?
        fileDir + fileName.substr(0, periodPos) + "/" :
        fileDir + fileName + "/";
    mkdir(keyFolder.c_str());
    Encryptor aes(keyFolder);
    close();
    return aes.encrypt(usePin, passcode, fileDir + fileName);
}

/**
 * Decrypt the file with either PIN or passwowrd
 * @param passcode - the PIN or password string
 * @return the Encryptor class generated enumeration result
 */
Encryptor::Status Data::decrypt(string passcode){
    int periodPos = fileName.find('.');
    std::string keyFolder = periodPos!=std::string::npos ?
        fileDir + fileName.substr(0, periodPos) + "/" :
        fileDir + fileName + "/";
    Encryptor aes(keyFolder);
    close();
    return aes.decrypt(passcode, fileDir + fileName);
}

/**
 * Encrypt/Decrypt the dummy file
 * @param state - encrypt(true) or decrypt(false)
 */
void Data::encrypt(bool state){
    int periodPos = fileName.find('.');
    std::string keyFolder = periodPos!=std::string::npos ?
        fileDir + fileName.substr(0, periodPos) + "/" :
        fileDir + fileName + "/";
    Encryptor obj(keyFolder);
    if (obj.isEncrypted(keyFolder + "key0.bin")!=state){
        obj.encryptFile(state, keyFolder + "key0.bin",
        "abcdefghijklmnop",
        "abcdefghijklmnop");
    }
    std::string defaultFile = "key0.bin";
    if (!state) changeFile(keyFolder, defaultFile);
}

/**
 * Gets the current full file location
 * @return the file directory, name, and extension
 */
string Data::getFileName(){
    return fileDir + fileName;
}

/**
 * Is the file in the file stream object encrypted?
 * @return whether it is encrypted
 */
bool Data::isEncrypted(){
    Encryptor obj;
    return obj.isEncrypted(fileDir + fileName);
}
