#ifndef VFS_ENCODING_H
#define VFS_ENCODING_H

#include <string>
#include <sstream>
#include <locale>

namespace MFFiles
{
    std::string convertPathToCanonical(std::string path)
    {
        std::stringstream sstream;

        // TODO(zaklaus): Find a more efficient way to deal with canonicalization of paths...

        std::locale loc;
        for (size_t i = 0; i < path.length(); ++i)
            sstream << std::tolower(path.at(i), loc);

        return sstream.str();
    }
}

#endif // VFS_ENCODING_H