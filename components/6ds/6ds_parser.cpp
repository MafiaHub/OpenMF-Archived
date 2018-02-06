#include <6ds/6ds_parser.hpp>
#include <string>

namespace MFFormat
{

bool DataFormat6DS::load(std::ifstream &srcFile)
{
    Header header = {};
    read(srcFile, &header);

    if (std::string(header.mMagic).compare("6DS") != 0)
    {
        return false;
    }

    for (uint32_t i = 0; i < header.mNumVertices; i++)
    {
        MFMath::Vec3 vertex = {};
        read(srcFile, &vertex);

        mVertices.push_back(vertex);
    }

    for (uint32_t i = 0; i < header.mNumVertexIndices / 3; i++)
    {
        Face face = {};
        read(srcFile, &face);

        mFaces.push_back(face);
    }

    for (uint32_t i = 0; i < header.mNumLinks; i++)
    {
        Link link = {};
        read(srcFile, &link.mNumVertices);
        read(srcFile, &link.mNumFaces);
        read(srcFile, &link.mNameLength);
        read(srcFile, &link.mName, link.mNameLength);

        mLinks.push_back(link);
    }

    return true;
}

}
