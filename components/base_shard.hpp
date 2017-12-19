#ifndef ENTITY_SHARD_H
#define ENTITY_SHARD_H

namespace MFEntity
{

class Entity;
    
class Shard
{
public:
    Shard() :   mOwner(nullptr), 
                mUserData(nullptr) {}
    virtual ~Shard() {}
    
    // TODO(zaklaus): Decide on what the pipeline should pass to the entity.
    virtual void onInit()=0;
    virtual void onInput()=0;
    virtual void onUpdate()=0;
    virtual void onRender()=0;
    virtual void onDebugDraw()=0;

    inline void setOwner(Entity *owner) { mOwner = owner; }
    inline void setUserData(void *userData) { mUserData = userData; }

protected:
    Entity *mOwner;
    void *mUserData;
};

}

#endif // ENTITY_SHARD_H