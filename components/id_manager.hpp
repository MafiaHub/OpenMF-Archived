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
        mIndex      = INT16_MAX;
        mGeneration = INT16_MAX;
    }

    _Id(uint16_t index, uint16_t generation)
    {
        mIndex = index;
        mGeneration = generation;
    }

    _Id(uint32_t value)
    {
        mValue = value;
    }

    struct
    {
        uint16_t mIndex;
        uint16_t mGeneration;
    };

    uint32_t mValue;

    bool operator==(const _Id rhs)
    {
        return (mIndex == rhs.mIndex && mGeneration == rhs.mGeneration);
    }

    bool operator!=(const _Id rhs)
    {
        return !(*this == rhs);
    }

    operator int(){ return static_cast<int>(mValue); }
} Id;

static Id NullId;

class IDManager 
{
public:
    virtual uint16_t getSlot(Id ident)=0;
    virtual bool isValid(Id ident)=0;
    virtual Id allocate()=0;
    virtual void deallocate(Id ident)=0;

protected:
    virtual Id *getHandle(Id ident, bool warn=true)=0;
};

}

#include <id_managers/backing_id_manager.hpp>

#endif // ID_MANAGER_H
