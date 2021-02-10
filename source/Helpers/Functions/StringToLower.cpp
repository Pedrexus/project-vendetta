#include "functions.h"

std::string StringToLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
        // static_cast<int(*)(int)>(std::tolower)         // wrong
        // [](int c){ return std::tolower(c); }           // wrong
        // [](char c){ return std::tolower(c); }          // wrong
        [](unsigned char c) { return std::tolower(c); } // correct
    );
    return s;
}