#ifndef LOGGER_CONSOLE_H
#define LOGGER_CONSOLE_H

#include <iostream>

#include <base_logger.hpp>

namespace MFLogger
{

class ConsoleLogger : public Logger 
{
public:
    virtual void print_raw  (std::string id, std::string str) override;
    virtual void print_info (std::string id, std::string str) override;
    virtual void print_warn (std::string id, std::string str) override;
    virtual void print_fatal(std::string id, std::string str) override;

	static ConsoleLogger *getInstance()
	{
		static ConsoleLogger logger;
		return &logger;
	}

    static void raw(std::string id, std::string str);
    static void info(std::string id, std::string str);
    static void warn(std::string id, std::string str);
    static void fatal(std::string id, std::string str);
};

void ConsoleLogger::raw(std::string id, std::string str)
{
	getInstance()->print_raw(id, str);
}

void ConsoleLogger::info(std::string id, std::string str)
{
	getInstance()->print_info(id, str);
}

void ConsoleLogger::warn(std::string id, std::string str)
{
	getInstance()->print_warn(id, str);
}

void ConsoleLogger::fatal(std::string id, std::string str)
{
	getInstance()->print_fatal(id, str);
}

void ConsoleLogger::print_raw(std::string id, std::string str)
{
	std::cout << str << std::endl;
}

void ConsoleLogger::print_info(std::string id, std::string str)
{
	if (canPrint(id, LOG_VERBOSITY_INFO))
		std::cout << "[INFO] " << str << std::endl;
}

void ConsoleLogger::print_warn(std::string id, std::string str)
{
	if (canPrint(id, LOG_VERBOSITY_WARN))
		std::cout << "[WARN] " << str << std::endl;
}

void ConsoleLogger::print_fatal(std::string id, std::string str)
{
	if (canPrint(id, LOG_VERBOSITY_FATAL))
		std::cout << "[FATAL] " << str << std::endl;
}

}

#endif // LOGGER_CONSOLE_H