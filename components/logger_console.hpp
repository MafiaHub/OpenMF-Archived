#ifndef LOGGER_CONSOLE_H
#define LOGGER_CONSOLE_H

#include <iostream>

#include <loggers.hpp>

namespace MFLogger
{

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