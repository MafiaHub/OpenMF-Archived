#include <check_bin/parser_checkbin.hpp>

namespace MFFormat
{

bool DataFormatCheckBIN::load(std::ifstream &srcFile)
{
    Header header = {};
    read(srcFile, &header);

    if (header.mMagic != 0x1ABCEDF)
    {
        return false;
    }

    uint32_t numLinks = 0;

    for (uint32_t i = 0; i < header.mNumPoints; i++)
    {
        Point point = {};
        read(srcFile, &point);

        numLinks += point.mEnterLinks;

        mPoints.push_back(point);
    }

    // Each point references 0 or more links.
    // For example, if point 0 has mEnterLinks = 2, it means that the first 2 links belong to it.
    // Consequent links in a row belong to point 1, 2 and so on.
    for (uint32_t i = 0; i < numLinks; i++)
    {
        Link link = {};
        read(srcFile, &link);

        mLinks.push_back(link);
    }

    return true;
}

}
