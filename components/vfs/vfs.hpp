#ifndef VFS_H
#define VFS_H

#include <fstream>
#include <string>
#include <vector>

#include <osdefines.hpp>
#include <dta/parser.hpp>
#include <vfs/encoding.hpp>

#ifdef OMF_SYSTEM_LINUX
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

namespace MFFiles
{

/**
    Meyers Singleton class that encapsulating working with files. Hides platform-dependant
    FS details and whether a file is inside DTA or on actual HDD.
*/

class Filesystem
{
public:
    ~Filesystem() {}

    static Filesystem *getInstance()
    {
        static Filesystem sFileSystem;
        return &sFileSystem;
    }

    void initDTA();

    bool open(std::ifstream &file, std::string fileName, std::ios_base::openmode mode = std::ios::binary);

    void                     addPath(std::string path) { mSearchPaths.push_back(path); }
    size_t                   getNumPaths()             { return mSearchPaths.size(); }
    std::vector<std::string> getPaths()          const { return mSearchPaths; }

private:
    Filesystem();                             // hide the constructor
    Filesystem(Filesystem const&);            // hide the copy constructor
    Filesystem& operator=(Filesystem const&); // hide the assign operator

    std::vector<std::string> mSearchPaths;
};
    
Filesystem::Filesystem()
{
    mSearchPaths.push_back("resources");
    mSearchPaths.push_back("mafia");

#if defined(OMF_SYSTEM_LINUX)
    struct passwd *pw = getpwuid(getuid());
    const char *dir = pw->pw_dir;
    std::string dirpath = std::string(dir) + "/";
    mSearchPaths.push_back(dirpath + ".openmf");
    mSearchPaths.push_back(dirpath + ".openmf/mafia");
#endif
};

void Filesystem::initDTA()
{

}

bool Filesystem::open(std::ifstream &file, std::string fileName, std::ios_base::openmode mode)
{
    fileName = convertPathToCanonical(fileName);

    for (auto path : mSearchPaths)
    {
        file.open(path + "/" + fileName, mode);
        if (file.is_open()) return true;
    }

    // TODO(zaklaus): Otherwise, search inside DTA archives...

    return false;
}

}

#endif // VFS_H
