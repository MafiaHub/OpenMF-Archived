#include <utils/bmp_analyser.hpp>

namespace MFFormat
{

bool BMPInfo::load(std::ifstream &f)
{
    char buffer[64];
    f.read(buffer,54);                      // headers after which there is a colortable
    f.read((char *) &mTransparentColor,4);  // the first color in the table is transparent
    return true;
}

}
