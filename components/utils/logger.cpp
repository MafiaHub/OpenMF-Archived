#include <utils/logger.hpp>
#include <iostream>

namespace MFLogger
{

std::vector<std::string> initVector()
{
    std::vector<std::string> result;
    return result;
}

bool Logger::sFilterExclusive = true;
uint32_t Logger::sVerbosityFlags = 0;
std::vector<std::string> Logger::sFilteredIDs = initVector();

bool Logger::canPrint(std::string id, uint32_t flags)
{
    if (!(flags & Logger::sVerbosityFlags))
        return false;

    return (std::find(Logger::sFilteredIDs.begin(), Logger::sFilteredIDs.end(), id) == Logger::sFilteredIDs.end()) == Logger::sFilterExclusive;
}

void Logger::removeFilter(std::string id)
{
    Logger::sFilteredIDs.erase(std::remove(Logger::sFilteredIDs.begin(), Logger::sFilteredIDs.end(), id), Logger::sFilteredIDs.end());
}

void Logger::raw(std::string str, std::string id)
{
    if (Logger::canPrint(id, LOG_VERBOSITY_FATAL))
        std::cout << str << std::endl;
}

void Logger::info(std::string str, std::string id)
{
    if (Logger::canPrint(id, LOG_VERBOSITY_INFO))
        std::cout << "[INFO] [" << id << "] " << str << std::endl;
}

void Logger::warn(std::string str, std::string id)
{
    if (Logger::canPrint(id, LOG_VERBOSITY_WARN))
        std::cout << "[WARN] [" << id << "] " << str << std::endl;
}

void Logger::fatal(std::string str, std::string id)
{
    if (Logger::canPrint(id, LOG_VERBOSITY_FATAL))
        std::cout << "[FATAL] [" << id << "] " << str << std::endl;
}

}
