#ifndef LOGGER_CONSOLE_H
#define LOGGER_CONSOLE_H

#include <iostream>

#include <loggers.hpp>

namespace MFLogger
{

void ConsoleLogger::print_info(std::string str)
{
    std::cout << "[INFO] " << str << std::endl;
}

void ConsoleLogger::print_warn(std::string str)
{
    std::cout << "[WARN] " << str << std::endl;
}

void ConsoleLogger::print_fatal(std::string str)
{
    std::cout << "[FATAL] " << str << std::endl;
}

}

#endif // LOGGER_CONSOLE_H