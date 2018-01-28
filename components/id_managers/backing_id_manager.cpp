#include <id_managers/backing_id_manager.hpp>

namespace MFGame
{

Id *BackingIDManager::getHandle(Id ident, bool warn)
{
    if (extractSlot(ident) > mIndices.size())
    {
        if (warn)
            MFLogger::ConsoleLogger::warn("ID is outside of the bounds.", ID_MANAGER_MODULE_STR);
        return nullptr;
    }
    else if (mIndices.at(extractSlot(ident)) != ident)
    {
        if (warn)
            MFLogger::ConsoleLogger::warn("Invalid ID.", ID_MANAGER_MODULE_STR);
        return nullptr;
    }

    auto id = &mIndices.at(extractSlot(ident));
    return id;
}

uint32_t BackingIDManager::getSlot(Id ident)
{
    auto id = getHandle(ident);

    if (id == nullptr)
    {
        return NullId;
    }

    return *id;
}

bool BackingIDManager::isValid(Id ident)
{
    auto id = getHandle(ident, false);

    return (id != nullptr);
}

Id BackingIDManager::allocate()
{
    if (mFreeIndices.size() > 0)
    {
        auto id = mFreeIndices.back();
        mFreeIndices.pop_back();
        return id;
    }

    Id id = constructId(mIndices.size(), 0);
    mIndices.push_back(id);

    if (extractSlot(id) == INT16_MAX)
    {
        MFLogger::ConsoleLogger::fatal("Maximum number of IDs has been reached!", ID_MANAGER_MODULE_STR);
        return NullId;
    }

    return id;
}

void BackingIDManager::deallocate(Id ident)
{
    auto id = getHandle(ident);
    if (id == nullptr)
    {
        MFLogger::ConsoleLogger::warn("Can't deallocate invalid ID.", ID_MANAGER_MODULE_STR);
        return;
    }

    *id = constructId(
        extractSlot(*id),
        extractGeneration(*id) + 1
    );
    mFreeIndices.push_back(*id);
}

}
