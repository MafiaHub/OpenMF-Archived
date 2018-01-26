#ifndef LOGGER_CONSOLE_H
#define LOGGER_CONSOLE_H

#include <iostream>

#include <base_logger.hpp>

namespace MFLogger
{

class ConsoleLogger : public Logger 
{
public:
    virtual void print_raw(std::string str, std::string id) override;
    virtual void print_info(std::string str, std::string id) override;
    virtual void print_warn(std::string str, std::string id) override;
    virtual void print_fatal(std::string str, std::string id) override;

    static ConsoleLogger *getInstance()
    {
        static ConsoleLogger logger(LOG_VERBOSITY_WARN | LOG_VERBOSITY_FATAL);
        return &logger;
    }

    static void raw(std::string str, std::string id = "");
    static void info(std::string str, std::string id = "");
    static void warn(std::string str, std::string id = "");
    static void fatal(std::string str, std::string id = "");

protected:
    ConsoleLogger();
    ConsoleLogger(uint32_t verbosityFlags);
};

}

#endif // LOGGER_CONSOLE_H
