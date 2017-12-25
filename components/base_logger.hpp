#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

namespace MFLogger
{

typedef enum
{
    LOG_VERBOSITY_INFO = 1,
    LOG_VERBOSITY_WARN = 2,
    LOG_VERBOSITY_FATAL = 4,
} LogVerbosity;

class Logger
{
public:
    virtual void print_raw  (std::string str, std::string id = "") = 0;
    virtual void print_info (std::string str, std::string id = "") = 0;
    virtual void print_warn (std::string str, std::string id = "") = 0;
    virtual void print_fatal(std::string str, std::string id = "") = 0;

    void setVerbosityFlags(uint32_t flags)
    {
        mVerbosityFlags = flags;
    }

    bool canPrint(std::string id, uint32_t flags)
    {
        if (!(flags & mVerbosityFlags))
            return false;

        return std::find(mFilteredIDs.begin(), mFilteredIDs.end(), id) == mFilteredIDs.end();
    }

    void addFilter(std::string id)
    {
        mFilteredIDs.push_back(id);
    }

    void removeFilter(std::string id)
    {
        mFilteredIDs.erase(std::remove(mFilteredIDs.begin(), mFilteredIDs.end(), id), mFilteredIDs.end());
    }

protected:
    uint32_t mVerbosityFlags;
    std::vector<std::string> mFilteredIDs;
};

}

#endif // LOGGER_H
