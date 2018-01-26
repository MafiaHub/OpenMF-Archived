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
    Logger() { mFilterExclusive = true; };
    virtual void print_raw  (std::string str, std::string id = "") = 0;
    virtual void print_info (std::string str, std::string id = "") = 0;
    virtual void print_warn (std::string str, std::string id = "") = 0;
    virtual void print_fatal(std::string str, std::string id = "") = 0;

    bool canPrint(std::string id, uint32_t flags);
    void setVerbosityFlags(uint32_t flags)    { mVerbosityFlags = flags;    }
    void addFilter(std::string id)            { mFilteredIDs.push_back(id); }
    void removeFilter(std::string id);

    /**
    Sets how the logger filter behaves.

    @param excludeByID If true, callers set with addFilter will be excluded from the log. False value will
                       revert the behavior, i.e. only callers added to the filter will be included.
    */

    void setFilterMode(bool excludeByID)      { mFilterExclusive = excludeByID; }

protected:
    uint32_t mVerbosityFlags;
    std::vector<std::string> mFilteredIDs;
    bool mFilterExclusive;   ///< says whether filter is exclusive of inclusive
};

}

#endif // LOGGER_H
