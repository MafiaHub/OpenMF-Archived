#include <id_managers/incrementing_id_manager.hpp>

namespace MFGame
{

Id IncrementingIDManager::allocate()
{
    if (mNextId == MFGame::NullId)
    {
        MFLogger::ConsoleLogger::fatal("Maximum number of IDs has been reached!", ID_MANAGER_MODULE_STR);
        return NullId;
    }

    return mNextId++;
}

}
