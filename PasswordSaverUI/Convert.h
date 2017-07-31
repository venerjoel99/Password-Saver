#ifndef CONVERT_H_INCLUDED
#define CONVERT_H_INCLUDED

class Convert{
public:
    static std::string intStr(int);
    static std::string charStr(char);
    static int charInt(char);
    static int strInt(std::string);
    static char intChar(int);
    static char strChar(std::string);
    static int digit(int, int);
    static int bitwiseXOR(int, int);
    static std::string arrayStr(char[], int);
};

#endif // CONVERT_H_INCLUDED
