#include <fstream>

namespace formats
{

/// Abstract class representing a game data format.

class DataFormat
{
public:
    virtual bool load(std::ifstream srcFile)=0;
    virtual bool save(std::ofstream dstFile)=0;
};

class DataFormat4DS: public DataFormat
{
};

class DataFormat5DS: public DataFormat
{
};

}
