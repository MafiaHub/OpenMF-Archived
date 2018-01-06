#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <string>
#include <sstream>
#include <iterator>
#include <vector>
#include <iomanip>

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

std::string doubleToStr(double value, unsigned int precision=2)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}

bool strStartsWith(std::string str, std::string prefix)
{
    return str.compare(0,prefix.size(),prefix) == 0;
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

    for (int i = 0; i < len; i++)
    {
        sstream << std::to_string(array[i]) << delim;
    }
    
    // TODO(zaklaus): erase last occurence of delimiter!

    return sstream.str();
}

size_t peekLength(std::ifstream &file)
{
    size_t len = 0;
    char currentChar;
    auto stateBefore = file.tellg();

    while(true) 
    {
        file.get(currentChar);
        len++;
        if(currentChar == '\0')
        {
            file.seekg(stateBefore, file.beg);
            return len;
        }    
    }
    return 0;
}

}

#endif
