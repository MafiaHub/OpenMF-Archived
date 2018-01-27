#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <string>
#include <sstream>
#include <iterator>
#include <vector>
#include <iomanip>
#include <cstring>

namespace MFUtil
{

std::string strToLower(std::string str);
std::string strToUpper(std::string str);
std::string doubleToStr(double value, unsigned int precision=2);
bool strStartsWith(std::string str, std::string prefix);

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

std::vector<std::string> strSplit(const std::string &s, char delim);
std::string strReverse(std::string s);

template<typename T>
std::string vecToString(const std::vector<T> &vec, std::string delim)
{
    std::stringstream sstream;

    for (auto value : vec)
    {
        sstream << std::to_string(value) << delim;
    }
    
    // TODO(zaklaus): erase last occurence of delimiter!

    return sstream.str();
}

template<typename T>
std::string arrayToString(T *array, size_t len, std::string delim)
{
    std::stringstream sstream;

    for (int i = 0; i < (int) len; i++)
    {
        sstream << std::to_string(array[i]) << delim;
    }
    
    // TODO(zaklaus): erase last occurence of delimiter!

    return sstream.str();
}

size_t peekLength(std::ifstream &file);

}

#endif
