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

}

#endif // LOGGER_H