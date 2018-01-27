#ifndef VFS_H
#define VFS_H

#include <fstream>
#include <string>
#include <vector>

#include <osdefines.hpp>
#include <dta/parser.hpp>

#include <loggers/console.hpp>

#ifdef OMF_SYSTEM_LINUX
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

#define VFS_MODULE_STR "VFS"
#define MAFIA_INSTALL_DIR "MAFIA_INSTALL_DIR"

namespace MFFile
{

std::string convertPathToCanonical(std::string path);

/**
    Meyers Singleton class that encapsulating working with files. Hides platform-dependant
    FS details and whether a file is inside DTA or on actual HDD.
*/

class FileSystem
{
public:
    ~FileSystem() {}

    static FileSystem *getInstance()
    {
        static FileSystem sFileSystem;
        return &sFileSystem;
    }

    bool open(std::ifstream &file, std::string fileName, std::ios_base::openmode mode = std::ios::binary);
    std::string getFileLocation(std::string fileName);

    void                     addPath(std::string path);
    void                     prependPath(std::string path);
    size_t                   getNumPaths()                   { return mSearchPaths.size(); }
    std::vector<std::string> getPaths()          const       { return mSearchPaths;        }

private:
    FileSystem();                             // hide the constructor
    FileSystem(FileSystem const&);            // hide the copy constructor
    FileSystem& operator=(FileSystem const&); // hide the assign operator
    void addPath(std::string path, size_t index);
    std::vector<std::string> mSearchPaths;
};

}

#endif // VFS_H
