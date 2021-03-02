#include "String.h"

std::vector<std::string> String::Split(char delimiter)
{
    std::vector<std::string> out;

    u64 start = 0;
    u64 len = m_string.size();
    while (start != m_string.size() - 1)
    {
        len = m_string.substr(start, std::string::npos).find(delimiter);   
        out.push_back(m_string.substr(start, len));
        start += len + 1;
    }

    out.push_back(m_string.substr(start, std::string::npos));

    return out;
}
