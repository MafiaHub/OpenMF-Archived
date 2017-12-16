#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

namespace MFLogger
{

class Logger 
{
public:
    virtual void print_raw(std::string str) = 0;
    virtual void print_info(std::string str) = 0;
    virtual void print_warn(std::string str) = 0;
    virtual void print_fatal(std::string str) = 0;
};

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

class FileLogger : Logger 
{
public:
    FileLogger(std::string fileName);
    virtual void print_raw(std::string str) override;
    virtual void print_info(std::string str) override;
    virtual void print_warn(std::string str) override;
    virtual void print_fatal(std::string str) override;

private:
    std::ofstream mFile;
};

}

#endif // LOGGER_H