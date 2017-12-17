#ifndef LOGGER_FILE_H
#define LOGGER_FILE_H

#include <base_logger.hpp>

namespace MFLogger
{

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

FileLogger::FileLogger(std::string fileName)
{
    mFile = std::ofstream(fileName, std::ios::out);
}

void FileLogger::print_info(std::string str)
{
    mFile << "[INFO] " << str << std::endl;
}

void FileLogger::print_warn(std::string str)
{
    mFile << "[WARN] " << str << std::endl;
}

void FileLogger::print_fatal(std::string str)
{
    mFile << "[FATAL] " << str << std::endl;
}

}

#endif // LOGGER_FILE_H