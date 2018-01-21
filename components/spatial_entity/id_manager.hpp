#ifndef ID_MANAGER_H
#define ID_MANAGER_H

#include <cstdint>
#include <limits.h>
#include <vector>
#include <loggers/console.hpp>

#define IDMANAGER_MODULE_STR "id manager"

namespace MFGame
{

typedef union _Id
{
    _Id() 
    {
        this->Value = (INT_MAX << 32) | INT_MAX;
    }

    _Id(uint32_t index, uint32_t generation)
    {
        this->Index = index;
        this->Generation = generation;
    }

    _Id(uint64_t value)
    {
        this->Value = value;
    }

    struct
    {
        uint32_t Index;
        uint32_t Generation;
    };

    uint64_t Value;

    bool operator==(const _Id rhs)
    {
        return (this->Index == rhs.Index && this->Generation == rhs.Generation);
    }

    bool operator!=(const _Id rhs)
    {
        return !(*this == rhs);
    }
} Id;

static Id NullId(INT_MAX, INT_MAX);

class IDManager 
{
public:
    virtual uint32_t getSlot(Id ident)=0;
    virtual bool isValid(Id ident)=0;
    virtual Id allocate()=0;
    virtual void deallocate(Id ident)=0;

protected:
    virtual Id *getHandle(Id ident, bool warn=true)=0;
};

class BackingIDManager: public IDManager
{
public:
    virtual uint32_t getSlot(Id ident) override;
    virtual bool isValid(Id ident) override;
    
    virtual Id allocate() override;
    virtual void deallocate(Id ident) override;

protected:
    virtual Id *getHandle(Id ident, bool warn=true) override;

private:
    std::vector<Id> mIndices;
    std::vector<Id> mFreeIndices;
};

Id *BackingIDManager::getHandle(Id ident, bool warn)
{
    if (ident.Index < 0 || ident.Index > mIndices.size())
    {
        if (warn)
            MFLogger::ConsoleLogger::warn("ID is outside of the bounds.", IDMANAGER_MODULE_STR);
        return nullptr;
    }
    else if (mIndices.at(ident.Index) != ident)
    {
        if (warn)
            MFLogger::ConsoleLogger::warn("Invalid ID.", IDMANAGER_MODULE_STR);
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

    if (id.Index == INT_MAX)
    {
        MFLogger::ConsoleLogger::fatal("Maximum number of IDs has been reached!", IDMANAGER_MODULE_STR);
        return NullId;
    }

    return id;
}

void BackingIDManager::deallocate(Id ident)
{
    auto id = getHandle(ident);
    id->Generation++;
    mFreeIndices.push_back(*id);
}

}

#endif // ID_MANAGER_H