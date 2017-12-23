#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

namespace MFLogger
{

typedef enum
{
	LOG_VERBOSITY_FATAL,
	LOG_VERBOSITY_WARN,
	LOG_VERBOSITY_INFO,
} LogVerbosity;

class Logger 
{
public:
    virtual void print_raw  (std::string str, std::string id = "") = 0;
    virtual void print_info (std::string str, std::string id = "") = 0;
    virtual void print_warn (std::string str, std::string id = "") = 0;
    virtual void print_fatal(std::string str, std::string id = "") = 0;

	void setVerbosity(uint8_t level)
	{
		mVerbosityLevel = level;
	}

	bool canPrint(std::string id, uint8_t level)
	{
		bool result = (mVerbosityLevel >= level);
		result = result && (std::find(mFilteredIDs.begin(), mFilteredIDs.end(), id) == mFilteredIDs.end());
		return result;
	}

	void addFilter(std::string id)
	{
		mFilteredIDs.push_back(id);
	}

	void removeFilter(std::string id)
	{
		mFilteredIDs.erase(std::remove(mFilteredIDs.begin(), mFilteredIDs.end(), id), mFilteredIDs.end());
	}

protected:
	uint8_t mVerbosityLevel;
	std::vector<std::string> mFilteredIDs;
};

}

#endif // LOGGER_H