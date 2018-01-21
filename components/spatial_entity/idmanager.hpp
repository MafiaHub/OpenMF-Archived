#ifndef IDMANAGER_H
#define IDMANAGER_H

#include <cstdint>
#include <limits.h>
#include <vector>
#include <loggers/console.hpp>

#define IDMANAGER_MODULE_STR "id manager"

namespace MFGame
{

typedef union _Id
{
    _Id(uint32_t Index, uint32_t Generation)
    {
        this->Index = Index;
        this->Generation = Generation;
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
    virtual uint32_t GetSlot(Id ident)=0;
    virtual Id Allocate()=0;
    virtual void Deallocate(Id ident)=0;

protected:
    virtual Id *GetHandle(Id ident)=0;
};

class BackingIDManager: public IDManager
{
public:
    virtual uint32_t GetSlot(Id ident) override;
    
    virtual Id Allocate() override;
    virtual void Deallocate(Id ident) override;

protected:
    virtual Id *GetHandle(Id ident) override;

private:
    std::vector<Id> mIndices;
    std::vector<Id> mFreeIndices;
};

Id *BackingIDManager::GetHandle(Id ident)
{
    if (ident.Index < 0 || ident.Index > mIndices.size())
    {
        MFLogger::ConsoleLogger::warn("ID is outside of the bounds.", IDMANAGER_MODULE_STR);
        return nullptr;
    }
    else if (mIndices.at(ident.Index) != ident)
    {
        MFLogger::ConsoleLogger::warn("Invalid ID.", IDMANAGER_MODULE_STR);
        return nullptr;
    }

    auto id = &mIndices.at(ident.Index);
    return id;
}

uint32_t BackingIDManager::GetSlot(Id ident)
{
    auto id = GetHandle(ident);

    if (id == nullptr)
    {
        return NullId.Index;
    }

    return id->Index;
}

Id BackingIDManager::Allocate()
{
    if (mFreeIndices.size() > 0)
    {
        auto id = mFreeIndices.back();
        mFreeIndices.pop_back();
        return id;
    }

    Id id(mIndices.size(), 0);
    mIndices.push_back(id);

    // TODO: Handle a case when we work with Id that uses INT_MAX index.

    return id;
}

void BackingIDManager::Deallocate(Id ident)
{
    auto id = GetHandle(ident);
    id->Generation++;
    mFreeIndices.push_back(*id);
}

}

#endif // IDMANAGER_H