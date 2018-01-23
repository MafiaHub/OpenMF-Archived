#ifndef ID_MANAGER_H
#define ID_MANAGER_H

#include <cstdint>
#include <limits.h>
#include <vector>
#include <loggers/console.hpp>

#define ID_MANAGER_MODULE_STR "id manager"

namespace MFGame
{

typedef uint32_t Id;

static Id NullId;

class IDManager 
{
public:
    virtual uint32_t getSlot(Id ident) { return ident; };
    virtual bool isValid(Id ident) { return true; };
    virtual Id allocate()=0;
    virtual void deallocate(Id ident) {};

protected:
    virtual Id *getHandle(Id ident, bool warn=true) { return nullptr; };
};

}

#include <id_managers/backing_id_manager.hpp>
#include <id_managers/incrementing_id_manager.hpp>

#endif // ID_MANAGER_H
