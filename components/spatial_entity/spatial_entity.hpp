#ifndef SPATIAL_ENTITY_H
#define SPATIAL_ENTITY_H

#include <string>

namespace MFGame
{

typedef struct
{
    float x;
    float y;
    float z;
} Vec3;

typedef struct
{
    float x;
    float y;
    float z;
} Quat;

/**
  Abstract interface for something that exists in 3D world and has a graphical and/or
  physical and/or sound etc. representations.
 */

class SpatialEntity
{
public:
    virtual void update(double dt)=0;
 
    Vec3 getPosition()                  { return mPosition;     };
    void setPosition(Vec3 position)     { mPosition = position; };   ///< Sets position without collisions.
    Quat getRotation()                  { return mRotation;     };
    void setRotation(Quat rotation)     { mRotation = rotation; };
    void setName(std::string name)      { mName = name;         };  
    std::string getName()               { return mName;         };

    /**
    Moves the entity from current to dest position, checking for collisions.
    */
    virtual void move(Vec3 destPosition)=0;

    /**
    Moves the entity by given vector, checking for collisions.
    */
//    void moveBy(Vec3 positionDiff);

protected:
    Vec3 mPosition;      
    Vec3 mScale;
    Quat mRotation;
    std::string mName;
};

};
#endif
