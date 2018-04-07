#include <utils/openmf.hpp>
#include <fstream>

namespace MFUtil
{

ScopedBuffer::ScopedBuffer(size_t size) : mData(0x0), mSize(size)
{
    mData = new char[mSize];
}

ScopedBuffer::ScopedBuffer(ScopedBuffer &&src) : mData(src.mData), mSize(src.mSize)
{
    src.mData = nullptr;
    src.mSize = 0;
}

ScopedBuffer::~ScopedBuffer()
{
    delete[] mData;
    mData = nullptr;
    mSize = 0;
}

char *ScopedBuffer::operator*()
{
    return mData;
}

const char *ScopedBuffer::operator*() const
{
    return mData;
}

char &ScopedBuffer::operator[](size_t idx)
{
    return mData[idx];
}

const char &ScopedBuffer::operator[](size_t idx) const
{
    return mData[idx];
}

ScopedBuffer::operator char *()
{
    return mData;
}

ScopedBuffer::operator const char *() const
{
    return mData;
}

size_t ScopedBuffer::size() const
{
    return mSize;
}

void ScopedBuffer::copy_from(size_t offset, const void *sptr, size_t ssize)
{
    if (offset + ssize > mSize)
        throw std::out_of_range("ScopedBuffer::copy_from");
    memcpy(mData + offset, sptr, ssize);
}

void dumpValue(std::string name, std::string value, int offset, bool useQuotes)
{
    printf("%*s\"%s\": %s,\n", offset * 4, " ", name.c_str(), useQuotes ? ("\"" + value + "\"").c_str() : value.c_str());
}

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

std::string doubleToStr(double value, unsigned int precision)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}

bool strStartsWith(std::string str, std::string prefix)
{
    return str.compare(0,prefix.size(),prefix) == 0;
}

std::vector<std::string> strSplit(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    _split(s, delim, std::back_inserter(elems));
    return elems;
}

std::string strReverse(std::string s)
{
    std::string reversed = std::string();
    reversed.resize(s.length());
    std::reverse_copy(s.begin(), s.end(), reversed.begin());
    return reversed;
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

