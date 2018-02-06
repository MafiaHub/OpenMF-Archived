#include <mnu/mnu_parser.hpp>

namespace MFFormat
{

bool DataFormatMNU::load(std::ifstream &srcFile)
{
    Header header = {};
    read(srcFile, &header);

    if (std::string(header.mMagic) != "Menu")
    {
        return false;
    }

    for (uint32_t i = 0; i < header.mNumControls; i++)
    {
        Control control = {};
        read(srcFile, &control);

        mControls.push_back(control);
    }

    return true;
}

bool DataFormatMenuDEF::load(std::ifstream &srcFile)
{
    while (srcFile)
    {
        Control control = {};
        read(srcFile, &control);

        if (srcFile)
            mControls.push_back(control);
    }

    return true;
}

}
