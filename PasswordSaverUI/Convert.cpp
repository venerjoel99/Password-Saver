#include "Convert.h"
#include <string>
#include <sstream>

/**
 * Takes a digit at a place value of the number
 * @param num - the number to take the digit from
 * @param place - the place value(10, 100, 1000, etc.)
 * @return the digit
 */
int Convert::digit(int num, int place){
    if(place!=1 && place%10!=0) return -1;
    return ((num%(place*10))/place);
}

/**
 * Takes two 4-digit numbers and
 * bitwise XOR each corresponding digit
 * @param num1, num2 - the two numbers to bitwise XOR
 * @return the resulting 4-digit number
 */
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

///Character to integer
int Convert::charInt(char digit){
    return (int)(digit - '0');
};

///Integer to character
char Convert::intChar(int num){
    return '0'+num;
}

///Alternative: std::to_string(int) [only c++ 11]
std::string Convert::intStr(int num){
    std::stringstream intStream;
    intStream << num;
    return intStream.str();
}
