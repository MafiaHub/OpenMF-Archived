#ifndef VFS_H
#define VFS_H

#include <fstream>
#include <string>
#include <vector>

#include <osdefines.hpp>
#include <dta/parser.hpp>

#ifdef OMF_SYSTEM_LINUX
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

namespace MFFiles
{

class Filesystem
{
public:
    Filesystem();
    ~Filesystem() {}

    void initDTA();

    bool open(std::ifstream &file, std::string fileName, std::ios_base::openmode mode = std::ios::binary);

    void                     addPath(std::string path) { mSearchPaths.push_back(path); }
    size_t                   getNumPaths()             { return mSearchPaths.size(); }
    std::vector<std::string> getPaths()          const { return mSearchPaths; }

private:
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
}

void Filesystem::initDTA()
{

}

bool Filesystem::open(std::ifstream &file, std::string fileName, std::ios_base::openmode mode)
{
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