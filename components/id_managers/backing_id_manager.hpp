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

  private:
    std::vector<Id> mIndices;
    std::vector<Id> mFreeIndices;
};

Id *BackingIDManager::getHandle(Id ident, bool warn)
{
    if (ident.Index < 0 || ident.Index > mIndices.size())
    {
        if (warn)
            MFLogger::ConsoleLogger::warn("ID is outside of the bounds.", ID_MANAGER_MODULE_STR);
        return nullptr;
    }
    else if (mIndices.at(ident.Index) != ident)
    {
        if (warn)
            MFLogger::ConsoleLogger::warn("Invalid ID.", ID_MANAGER_MODULE_STR);
        return nullptr;
    }

    auto id = &mIndices.at(ident.Index);
    return id;
}

uint32_t BackingIDManager::getSlot(Id ident)
{
    auto id = getHandle(ident);

    if (id == nullptr)
    {
        return NullId.Index;
    }

    return id->Index;
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

    if (id.Index == INT32_MAX)
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

    id->Generation++;
    mFreeIndices.push_back(*id);
}

}

#endif // BACKING_ID_MANAGER_H