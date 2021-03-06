#include "data.h"

#include <fstream>
#include <string>
#include <iostream>

#include "Encryptor.h"

///Constructor for Info class
Info::Info(std::string website, std::string username, std::string password){
    this->website = website;
    this->username = username;
    this->password = password;
}

/**
 * Static function for prompting user input
 * @return The user's input's string
 */
std::string Info::input(){
    std::string info;
    std::cin.sync();
    getline(std::cin, info);
    return info;
}

/**
 * Converts info object into std::string object
 * @return Converted info string
 */
std::string Info::toStr(){
    std::string infoString =
    "Website: " + website + "\n"
    + "Username: " + username + "\n"
    + "Password: " + password + "\n";
    return infoString;
}

///Writes objects into text files
std::fstream& operator<<(std::fstream& streaming, Info info){
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
int findSize(std::fstream& file){
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

///Inputs text file content into string
std::fstream& operator>>(std::fstream& stream, std::string& fileString){
    if (stream.is_open()){
        fileString = "";
        char stringCreator;
        int fileSize = findSize(stream);
        for (int i = 0; i < fileSize; i++){
            stream.seekg(i);
            stream.get(stringCreator);
            if ((int)stringCreator==0) {
                continue;
            }
            fileString += stringCreator;
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
}

Data::Data(std::string dir, std::string file){
    fileDir = dir;
    fileName = file;
}

/**
 * Default constructor for Data class
 */
Data::Data(){
    fileDir = "";
    fileName = "";
}

/**
 * Modified destructor for the Data class
 */
Data::~Data(){
    if (theFile.is_open()) theFile.close();
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
    std::string fullFile = fileDir + fileName;
    switch(mode){
        case REGULAR:
            theFile.open(fullFile.c_str(), std::ios::out | std::ios::in | std::ios::app | std::ios::binary);
            break;
        case ENCRYPTION:
            theFile.open(fullFile.c_str(), std::ios::out | std::ios::in | std::ios::binary);
            break;
        case REWRITE:
            theFile.open(fullFile.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
            break;
        case READ_ONLY:
            theFile.open(fullFile.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
            break;
        default:
            theFile.open(fullFile.c_str(), std::ios::out | std::ios::in | std::ios::app | std::ios::binary);
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
int Data::searchEngine(std::string keyword){
    if (!open(REGULAR)) return -1;
    std::string result;
    int index = 0;
    int fileSize = findSize();
    int length = keyword.size();
    char compared, character, newLine;
    for(int i = 0; i<fileSize; i++){
        compared = keyword.at(index);
        if (i==fileSize-1){
            return -1;
        }
        theFile.seekg(i, std::ios::beg);
        theFile.get(character);
        if (character==compared && i > 0){
            theFile.seekg(i-1, std::ios::beg);
            theFile.get(newLine);
            if ((int)newLine!=0 && newLine!='\n') continue;
        }
        while ((compared==character)&&(index<length)){
            compared = keyword.at(index);
            theFile.seekg(i + index);
            theFile.get(character);
            if (index==0){result = character;}
            else {result += character;}
            index++;
        }
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
    std::string website, username, password;
    if (open(REGULAR) && position >= 0){
        int enterCount = 0;
        int consecutiveEnters = 0;
        char indicator;
        website = username = password = "null";
        position--;
        while (enterCount < 3){
            position++;
            theFile.seekg(position);
            theFile.get(indicator);
            if (indicator=='\n'){
                enterCount++;
                consecutiveEnters++;
                if (consecutiveEnters > 1){
                    Info nullResult("Null", "No", "Result");
                    return nullResult;
                }
            }
            else {
                consecutiveEnters = 0;
                if (enterCount==0){
                    if (website=="null"){
                        website = indicator;
                    }
                    else website +=indicator;
                }

                else if(enterCount==1){
                    if (username=="null"){
                        username = indicator;
                    }
                    else username +=indicator;
                }
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
        theFile.seekg(0, std::ios::beg);
        beginning = theFile.tellg();
        theFile.seekg(0, std::ios::end);
        ending = theFile.tellg();
        return ending - beginning;
    }
    return -1;
}

/**
 * Public function to return an Info data structure
 * from inserting a search keyword
 * @param searchWord - the keyword to search in the file
 * @return the Info data structure result if any
 */
Info Data::findSearch(std::string searchWord){
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
 * Write a string into the file stream
 * @param str - the file string
 * @param rewrite - whether to clear the previous data
 * @return the resulting status of the function
 */
Data::Success Data::writeIntoFile(std::string str, bool rewrite){
    bool opened = rewrite ? open(REWRITE) : open(REGULAR);
    if (opened){
        theFile << str;
        return SUCCESS;
    }
    return FILE_NOT_FOUND;
}

/**
 * Read the file content into a std::string object
 * @param datastd::string - the std::string object to insert file content into
 * @return the resulting status of the function
 */
Data::Success Data::readFromFile(std::string& dataString){
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
Data::Success Data::changeInfo(std::string website, std::string info, Change flag){
    std::string fileString;
    Success result = readFromFile(fileString);
    if (result!=SUCCESS){
        return result;
    }
    int position = -1;
    int enterCount = 0;
    char indicator;
    position = searchEngine(website);
    if (position==-1) return NO_RESULTS;
    std::string sub1 = fileString.substr(0,position);
    Info old = read(position);
    while (enterCount<4){
        position++;
        theFile.seekg(position);
        theFile.get(indicator);
        enterCount += indicator=='\n' ? 1 : 0;
    }
    position++;
    std::string sub2 = fileString.substr(position, fileString.size()-position);
    Info newInfo(old.getWebsite(),
                 (flag==US) ? info : old.getUsername(),
                 (flag==PS) ? info : old.getPassword());
    std::string newFileString;
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
Data::Success Data::changeInfo(std::string website, std::string newUsername, std::string newPassword){
    Success stat = changeInfo(website, newUsername, US);
    if (stat!=SUCCESS) return stat;
    return changeInfo(website, newPassword, PS);
}



/**
 * Gets the current full file location
 * @return the file directory, name, and extension
 */
std::string Data::getFileName(){
    return fileDir + fileName;
}
