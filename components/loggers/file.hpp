#ifndef LOGGER_FILE_H
#define LOGGER_FILE_H

#include <base_logger.hpp>

namespace MFLogger
{

class FileLogger : public Logger 
{
public:
    FileLogger(std::string fileName);
    virtual void print_raw  (std::string id, std::string str) override;
    virtual void print_info (std::string id, std::string str) override;
    virtual void print_warn (std::string id, std::string str) override;
    virtual void print_fatal(std::string id, std::string str) override;

private:
    std::ofstream mFile;
};

}

#endif // LOGGER_FILE_H
