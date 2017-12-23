#ifndef LOGGER_CONSOLE_H
#define LOGGER_CONSOLE_H

#include <iostream>

#include <base_logger.hpp>

namespace MFLogger
{

class ConsoleLogger : public Logger 
{
public:
    virtual void print_raw  (std::string str, std::string id) override;
    virtual void print_info (std::string str, std::string id) override;
    virtual void print_warn (std::string str, std::string id) override;
    virtual void print_fatal(std::string str, std::string id) override;

	static ConsoleLogger *getInstance()
	{
		static ConsoleLogger logger;
		return &logger;
	}

    static void raw(std::string str, std::string id = "");
    static void info(std::string str, std::string id = "");
    static void warn(std::string str, std::string id = "");
    static void fatal(std::string str, std::string id = "");
};

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
		std::cout << "[INFO] " << str << std::endl;
}

void ConsoleLogger::print_warn(std::string str, std::string id)
{
	if (canPrint(id, LOG_VERBOSITY_WARN))
		std::cout << "[WARN] " << str << std::endl;
}

void ConsoleLogger::print_fatal(std::string str, std::string id)
{
	if (canPrint(id, LOG_VERBOSITY_FATAL))
		std::cout << "[FATAL] " << str << std::endl;
}

}

#endif // LOGGER_CONSOLE_H