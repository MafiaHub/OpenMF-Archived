#ifndef SPATIAL_ENTITY_IMPLEMENTATION_H
#define SPATIAL_ENTITY_IMPLEMENTATION_H

#include <spatial_entity/spatial_entity.hpp>
#include <osg/Node>

namespace MFGame
{

class SpatialEntityImplementation: public SpatialEntity
{
public:
    virtual void update(double dt) override;
    virtual void move(Vec3 destPosition) override;

protected:
    osg::ref_ptr<osg::Node> mOSGNode;
};

void SpatialEntityImplementation::update(double dt)
{
}

void SpatialEntityImplementation::move(Vec3 destPosition)
{
}

};

#endif
