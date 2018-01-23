#ifndef BACKING_ID_MANAGER_H
#define BACKING_ID_MANAGER_H

namespace MFGame
{

class BackingIDManager : public IDManager
{
  public:
    virtual uint32_t getSlot(Id ident) override;
    virtual bool isValid(Id ident) override;

    virtual Id allocate() override;
    virtual void deallocate(Id ident) override;

  protected:
    virtual Id *getHandle(Id ident, bool warn = true) override;
    uint16_t extractSlot(Id ident)       { return (uint16_t)(ident >> 16); }
    uint16_t extractGeneration(Id ident) { return (uint16_t) ident; }
    uint32_t constructId(uint16_t slot, uint16_t generation) { return (uint32_t)slot << 16 | generation; }

  private:
    std::vector<Id> mIndices;
    std::vector<Id> mFreeIndices;
};

Id *BackingIDManager::getHandle(Id ident, bool warn)
{
    if (extractSlot(ident) < 0 || extractSlot(ident) > mIndices.size())
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

#endif // BACKING_ID_MANAGER_H
