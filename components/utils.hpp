#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <string>
#include <sstream>
#include <iterator>
#include <vector>

namespace MFUtil
{

std::string strToLower(std::string str)
{
    std::string result = str;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

std::string strToUpper(std::string str)
{
    std::string result = str;
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

template<typename Out>
void _split(const std::string &s, char delim, Out result)
{
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delim))
    {
        *(result++) = item;
    }
}

std::vector<std::string> strSplit(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    _split(s, delim, std::back_inserter(elems));
    return elems;
}

}

#endif
