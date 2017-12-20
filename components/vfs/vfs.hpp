#ifndef VFS_H
#define VFS_H

#include <fstream>
#include <string>
#include <vector>

#include <osdefines.hpp>
#include <dta/parser.hpp>
#include <vfs/encoding.hpp>

#include <loggers/console.hpp>

#ifdef OMF_SYSTEM_LINUX
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

namespace MFFile
{

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

    void initDTA();

    bool open(std::ifstream &file, std::string fileName, std::ios_base::openmode mode = std::ios::binary);
    std::string getFileLocation(std::string fileName);

    void                     addPath(std::string path);
    size_t                   getNumPaths()             { return mSearchPaths.size(); }
    std::vector<std::string> getPaths()          const { return mSearchPaths; }

private:
    FileSystem();                             // hide the constructor
    FileSystem(FileSystem const&);            // hide the copy constructor
    FileSystem& operator=(FileSystem const&); // hide the assign operator

    std::vector<std::string> mSearchPaths;
};
    
FileSystem::FileSystem()
{
    mSearchPaths.push_back("resources");
    mSearchPaths.push_back("mafia");

#if defined(OMF_SYSTEM_LINUX)
    struct passwd *pw = getpwuid(getuid());
    const char *dir = pw->pw_dir;
    std::string dirpath = std::string(dir) + "/";
    addPath(dirpath + ".openmf");
    addPath(dirpath + ".openmf/mafia");
#endif
};

void FileSystem::addPath(std::string path)
{
    if (path[path.length() - 1] == '/')
        path.erase(path.length() - 1,1);            

    mSearchPaths.push_back(path);
}

void FileSystem::initDTA()
{

}

std::string FileSystem::getFileLocation(std::string fileName)
{
    for (auto path : mSearchPaths)     // TODO: what's the fastest way to check a file existence?
    {
        std::string realPath = path + "/" + fileName;

        std::ifstream f;
        f.open(realPath);

        if (f.good())
        {
            f.close();
            return realPath;
        }
    }

    return "";
}

bool FileSystem::open(std::ifstream &file, std::string fileName, std::ios_base::openmode mode)
{
    // fileName = convertPathToCanonical(fileName);
    /* NOTE(drummy): For now that the files are loaded from HDD converting to lowercase
       breaks loading on Linux, later re-enable this. */

    std::string fileLocation = getFileLocation(fileName);    

    if (fileLocation.length() == 0)
        return false;

    file.open(fileLocation);
    return file.good();

    // TODO(zaklaus): Otherwise, search inside DTA archives...
}

}

#endif // VFS_H
