#ifndef BMP_ANALYSER_H
#define BMP_ANALYSER_H

#include <fstream>

namespace MFFormat
{

class BMPInfo
{
public:
    typedef struct
    {
        unsigned char b;
        unsigned char g;
        unsigned char r;
        unsigned char unused;
    } BMPColor;

    bool load(std::ifstream &f);

    BMPColor mTransparentColor;
};

bool BMPInfo::load(std::ifstream &f)
{
    char buffer[64];
    f.read(buffer,54);                      // headers after which there is a colortable
    f.read((char *) &mTransparentColor,4);  // the first color in the table is transparent
}

}

#endif
