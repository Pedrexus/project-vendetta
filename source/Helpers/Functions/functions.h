#pragma once

#include <pch.h>

extern void SetMemoryChecks();
extern std::string StringToLower(std::string s);
extern bool WildcardMatch(const char* pat, const char* str);
extern std::wstring ConvertANSIToUNICODE(const char* source, const unsigned int length);