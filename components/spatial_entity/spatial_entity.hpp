#ifndef SPATIAL_ENTITY_H
#define SPATIAL_ENTITY_H

#include <string>

namespace MFGame
{

typedef struct Vec3s
{
    float x;
    float y;
    float z;

    Vec3s(): x(0), y(0), z(0) {};
    Vec3s(float initX, float initY, float initZ): x(initX), y(initY), z(initZ) {};
    struct Vec3s operator+(struct Vec3s v) { struct Vec3s r; r.x = x + v.x; r.y = y + v.y; r.z = z + v.z; return r; };
    struct Vec3s operator-(struct Vec3s v) { struct Vec3s r; r.x = x - v.x; r.y = y - v.y; r.z = z - v.z; return r; };
    struct Vec3s operator*(float v) { struct Vec3s r; r.x = x * v, r.y = y * v; r.z = z * v; return r; };
    std::string str() { return "[" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + "]"; };
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
    SpatialEntity();

    virtual void update(double dt)=0;

    /**
      Initialises the entity with currently set data (OSG node, Bullet body, ...) so that it
      is ready to work.
    */
    virtual void ready()=0;
   
    virtual std::string toString()      { return "";            };
 
    Vec3 getPosition()                  { return mPosition;     };
    void setPosition(Vec3 position)     { mPosition = position; };   ///< Sets position without collisions.
    Quat getRotation()                  { return mRotation;     };
    void setRotation(Quat rotation)     { mRotation = rotation; };
    void setName(std::string name)      { mName = name;         };  
    std::string getName()               { return mName;         };
    bool isRead()                       { return mReady;        };

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
    bool mReady;
};

SpatialEntity::SpatialEntity()
{
    mPosition = Vec3();
    mScale = Vec3(1,1,1);
    mReady = true;
}

};
#endif
