#ifndef SHARD_SPHERE_RENDERER
#define SHARD_SPHERE_RENDERER

#include <base_shard.hpp>
#include <osg/ShapeDrawable>

namespace MFEntity
{

class SphereRenderer : public Shard
{
public:
    SphereRenderer()  {}
    ~SphereRenderer()
    {
        if (mSphereNode)
            mOwner->getGroup()->removeChild(mSphereNode);
    }

    virtual void onInit() override
    {
        mSphereNode = new osg::ShapeDrawable(
                        new osg::Sphere(osg::Vec3f(0,0,0),0.1));

        mOwner->getGroup()->addChild(mSphereNode);
    }

    virtual void onInput() override {}
    virtual void onUpdate() override
    {
        
    }

    virtual void onRender() override {}
    virtual void onDebugDraw() override {}

private:
    osg::ref_ptr<osg::ShapeDrawable> mSphereNode;
};

}

#endif // SHARD_SPHERE_RENDERER