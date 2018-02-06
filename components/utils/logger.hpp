#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <string>

namespace MFLogger
{

typedef enum
{
    LOG_VERBOSITY_INFO =  1,
    LOG_VERBOSITY_WARN =  2,
    LOG_VERBOSITY_FATAL = 4,
} LogVerbosity;

class Logger
{
public:
    static void raw  (std::string str, std::string id = "");
    static void info (std::string str, std::string id = "");
    static void warn (std::string str, std::string id = "");
    static void fatal(std::string str, std::string id = "");

    static bool canPrint(std::string id, uint32_t flags);
    static void setVerbosityFlags(uint32_t flags)    { Logger::sVerbosityFlags = flags;        }
    static void addFilter(std::string id)            { Logger::sFilteredIDs.push_back(id);     }
    static void removeFilter(std::string id);

    /**
    Sets how the logger filter behaves.
    @param excludeByID If true, callers set with addFilter will be excluded from the log. False value will
    revert the behavior, i.e. only callers added to the filter will be included.
    */

    static void setFilterMode(bool excludeByID)      { Logger::sFilterExclusive = excludeByID; }

    static uint32_t sVerbosityFlags;
    static std::vector<std::string> sFilteredIDs;
    static bool sFilterExclusive;   ///< says whether filter is exclusive of inclusive
};

}

#endif // LOGGER_H
