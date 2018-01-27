#include <base_logger.hpp>

namespace MFLogger
{

bool Logger::canPrint(std::string id, uint32_t flags)
{
    if (!(flags & mVerbosityFlags))
        return false;

    return (std::find(mFilteredIDs.begin(), mFilteredIDs.end(), id) == mFilteredIDs.end()) == mFilterExclusive;
}

void Logger::removeFilter(std::string id)
{
    mFilteredIDs.erase(std::remove(mFilteredIDs.begin(), mFilteredIDs.end(), id), mFilteredIDs.end());
}

}
