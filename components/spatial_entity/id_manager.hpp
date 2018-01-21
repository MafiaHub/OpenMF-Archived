#ifndef ID_MANAGER_H
#define ID_MANAGER_H

#include <cstdint>
#include <limits.h>
#include <vector>
#include <loggers/console.hpp>

#define ID_MANAGER_MODULE_STR "id manager"

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

static Id NullId;

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

}

#include <spatial_entity/backing_id_manager.hpp>

#endif // ID_MANAGER_H