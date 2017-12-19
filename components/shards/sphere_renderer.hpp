#ifndef SHARD_SPHERE_RENDERER
#define SHARD_SPHERE_RENDERER

#include <base_shard.hpp>

namespace MFEntity
{

class SphereRenderer : public Shard
{
public:
    SphereRenderer() {}
    ~SphereRenderer();

    virtual void onInit() override {}
    virtual void onInput() override {}
    virtual void onUpdate() override {}
    virtual void onRender() override {}
    virtual void onDebugDraw() override {}
}

}

#endif // SHARD_SPHERE_RENDERER