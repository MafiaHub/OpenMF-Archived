#ifndef VFS_ENCODING_H
#define VFS_ENCODING_H

#include <utils.hpp>

namespace MFFile
{
    std::string convertPathToCanonical(std::string path)
    {
        return MFUtil::strToLower(path);
    }
}

#endif // VFS_ENCODING_H
