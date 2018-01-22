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
        mIndex      = INT32_MAX;
        mGeneration = INT32_MAX;
    }

    _Id(uint32_t index, uint32_t generation)
    {
        mIndex = index;
        mGeneration = generation;
    }

    _Id(uint64_t value)
    {
        mValue = value;
    }

    struct
    {
        uint32_t mIndex;
        uint32_t mGeneration;
    };

    uint64_t mValue;

    bool operator==(const _Id rhs)
    {
        return (mIndex == rhs.mIndex && mGeneration == rhs.mGeneration);
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

}

#include <id_managers/backing_id_manager.hpp>

#endif // ID_MANAGER_H
