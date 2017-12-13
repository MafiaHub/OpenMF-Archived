#include <fstream>
#include <iostream>
#include <cstdint>

namespace formats
{

/// Abstract class representing a game data format.

class DataFormat
{
public:
    virtual bool load(std::ifstream &srcFile)=0;
    virtual bool save(std::ofstream &dstFile)=0;
};

class DataFormat4DS: public DataFormat
{
};

class DataFormat5DS: public DataFormat
{
};

class DataFormatDTA: public DataFormat
{
public:
    virtual bool load(std::ifstream &srcFile) override;
    virtual bool save(std::ofstream &dstFile) override;
};

bool DataFormatDTA::load(std::ifstream &srcFile)
{
    int32_t header;
    srcFile.read((char *) &header,4);

    if (header != 0x30445349)
      return false;

    return true;
}

bool DataFormatDTA::save(std::ofstream &srcFile)
{
    return true;
}

}
