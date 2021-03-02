#pragma once

#include <pch.h>
#include <types.h>


class String
{
    std::string m_string;

public:
    inline String(char* str) : m_string(std::string(str)) {}
    inline String(const char* str) : m_string(std::string(str)) {}
    inline String(std::string str) : m_string(str) {}
    inline ~String() = default;

    std::vector<std::string> Split(char delimiter);
};