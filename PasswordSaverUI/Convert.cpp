#include <string>
#include <sstream>
#include "Convert.h"

using namespace std;

string Convert::intStr(int num){
    stringstream intStream;
    intStream << num;
    return intStream.str();
}
string Convert::charStr(char letter){
    string charStr;
    charStr = letter;
    return charStr;
};
int Convert::charInt(char digit){
    return (int)(digit - '0');
};
int Convert::strInt(string password){
    stringstream pin(password);
    int value;
    pin >> value;
    return value;
}
char Convert::intChar(int num){
    return '0'+num;
}

char Convert::strChar(string str){
    return str.at(0);
};

string Convert::arrayStr(char arr[], int arrSize){
    string result = "null";
    for (int i = 0; i < arrSize; i++){
        if (result=="null") {
            result = arr[i];             //If not created yet, the first character is assigned to the string
                continue;                               //Makes sure that it does not go on to adding the first character to the string twice
        }
        result += arr[i];
    }
    return result;
}

int Convert::digit(int num, int place){
    if(place!=1 && place%10!=0) return -1;
    return ((num%(place*10))/place);
}

int Convert::bitwiseXOR(int num1, int num2){
    if (num1<0 || num1>10000
        || num2<0 ||num2>10000){
        return -1;
    }
    int result = 0;
    for (int i = 1000; i > 0; i/=10){
        int value = Convert::digit(num1,i)^Convert::digit(num2,i);
        value*=i;
        result+=value;
    }
    return result;
}
