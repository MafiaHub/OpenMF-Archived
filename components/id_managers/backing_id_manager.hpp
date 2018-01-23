#ifndef BACKING_ID_MANAGER_H
#define BACKING_ID_MANAGER_H

namespace MFGame
{

class BackingIDManager : public IDManager
{
  public:
    virtual uint16_t getSlot(Id ident) override;
    virtual bool isValid(Id ident) override;

    virtual Id allocate() override;
    virtual void deallocate(Id ident) override;

  protected:
    virtual Id *getHandle(Id ident, bool warn = true) override;

  private:
    std::vector<Id> mIndices;
    std::vector<Id> mFreeIndices;
};

Id *BackingIDManager::getHandle(Id ident, bool warn)
{
    if (ident.mIndex < 0 || ident.mIndex > mIndices.size())
    {
        if (warn)
            MFLogger::ConsoleLogger::warn("ID is outside of the bounds.", ID_MANAGER_MODULE_STR);
        return nullptr;
    }
    else if (mIndices.at(ident.mIndex) != ident)
    {
        if (warn)
            MFLogger::ConsoleLogger::warn("Invalid ID.", ID_MANAGER_MODULE_STR);
        return nullptr;
    }

    auto id = &mIndices.at(ident.mIndex);
    return id;
}

uint16_t BackingIDManager::getSlot(Id ident)
{
    auto id = getHandle(ident);

    if (id == nullptr)
    {
        return NullId.mIndex;
    }

    return id->mIndex;
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

    Id id(mIndices.size(), 0);
    mIndices.push_back(id);

    if (id.mIndex == INT16_MAX)
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

    id->mGeneration++;
    mFreeIndices.push_back(*id);
}

}

#endif // BACKING_ID_MANAGER_H
