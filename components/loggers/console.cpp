#include <loggers/console.hpp>

namespace MFLogger
{

ConsoleLogger::ConsoleLogger(): Logger()
{
}

ConsoleLogger::ConsoleLogger(uint32_t verbosityFlags): Logger()
{
    mVerbosityFlags = verbosityFlags;
}

void ConsoleLogger::raw(std::string str, std::string id)
{
    getInstance()->print_raw(str, id);
}

void ConsoleLogger::info(std::string str, std::string id)
{
    getInstance()->print_info(str, id);
}

void ConsoleLogger::warn(std::string str, std::string id)
{
    getInstance()->print_warn(str, id);
}

void ConsoleLogger::fatal(std::string str, std::string id)
{
    getInstance()->print_fatal(str, id);
}

void ConsoleLogger::print_raw(std::string str, std::string id)
{
    std::cout << str << std::endl;
}

void ConsoleLogger::print_info(std::string str, std::string id)
{
    if (canPrint(id, LOG_VERBOSITY_INFO))
        std::cout << "[INFO] [" << id << "] " << str << std::endl;
}

void ConsoleLogger::print_warn(std::string str, std::string id)
{
    if (canPrint(id, LOG_VERBOSITY_WARN))
        std::cout << "[WARN] [" << id << "] " << str << std::endl;
}

void ConsoleLogger::print_fatal(std::string str, std::string id)
{
    if (canPrint(id, LOG_VERBOSITY_FATAL))
        std::cout << "[FATAL] [" << id << "] " << str << std::endl;
}

}
