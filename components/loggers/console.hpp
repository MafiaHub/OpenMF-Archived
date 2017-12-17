#ifndef LOGGER_CONSOLE_H
#define LOGGER_CONSOLE_H

#include <iostream>

#include <base_logger.hpp>

namespace MFLogger
{

class ConsoleLogger : Logger 
{
public:
    virtual void print_raw(std::string str) override;
    virtual void print_info(std::string str) override;
    virtual void print_warn(std::string str) override;
    virtual void print_fatal(std::string str) override;

    static void raw(std::string str);
    static void info(std::string str);
    static void warn(std::string str);
    static void fatal(std::string str);
};

void ConsoleLogger::raw(std::string str)
{
    std::cout << str << std::endl;
}

void ConsoleLogger::info(std::string str)
{
    std::cout << "[INFO] " << str << std::endl;
}

void ConsoleLogger::warn(std::string str)
{
    std::cout << "[WARN] " << str << std::endl;
}

void ConsoleLogger::fatal(std::string str)
{
    std::cout << "[FATAL] " << str << std::endl;
}

void ConsoleLogger::print_raw(std::string str)
{
    ConsoleLogger::raw(str);
}

void ConsoleLogger::print_info(std::string str)
{
    ConsoleLogger::info(str);
}

void ConsoleLogger::print_warn(std::string str)
{
    ConsoleLogger::warn(str);
}

void ConsoleLogger::print_fatal(std::string str)
{
    ConsoleLogger::fatal(str);
}

}

#endif // LOGGER_CONSOLE_H