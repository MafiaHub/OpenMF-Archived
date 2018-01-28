#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <string>
#include <sstream>
#include <iterator>
#include <vector>
#include <iomanip>
#include <cstring>
#include <assert.h>
#include <cstddef>     // for size_t
#include <string.h>
#include <stdexcept>

namespace MFUtil
{

/// Non-Copyable (move passable) utilitary buffer with RAII scoping

class ScopedBuffer
{
public:
    ScopedBuffer(size_t size);
    ScopedBuffer(ScopedBuffer &&src);
    ~ScopedBuffer();
    char *operator*();
    const char *operator*() const;
    char &operator[](size_t idx);
    const char &operator[](size_t idx) const;
    operator char *();
    operator const char *() const;
    size_t size() const;

    template<typename TypeT>
    TypeT *as()
    {
        return reinterpret_cast<TypeT*>(mData);
    }

    template<typename TypeT>
    const TypeT *as() const
    {
        return reinterpret_cast<TypeT*>(mData);
    }

    // copies specified data to this instance, starting write at given offset
    void copy_from(size_t offset, const void *sptr, size_t ssize);

private:
    ScopedBuffer(const ScopedBuffer &other) = delete;
    ScopedBuffer &operator=(const ScopedBuffer &other) = delete;

    char *mData;
    size_t mSize;
};

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
