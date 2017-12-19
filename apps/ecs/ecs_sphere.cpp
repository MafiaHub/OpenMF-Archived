// ECS Sphere renderer test

#include <entity.hpp>
#include <shards/sphere_renderer.hpp>
#include <renderer/renderer_osg.hpp>

int main()
{
    auto sphere = new MFEntity::Entity();
    auto sphereRenderer = new MFEntity::SphereRenderer();
    sphere->assign(sphereRenderer);

    auto root = new MFEntity::Entity();
    root->addChild(sphere);
    root->onInit();

    MFRender::OSGRenderer renderer;
    renderer.setSceneData(root->getGroup());

    // NOTE(zaklaus): Uncomment to remove shard from an entity.
    // sphere->dispose(sphereRenderer);

    while(!renderer.done())
    {
        root->onInput();
        root->onUpdate();
        root->onRender();
        renderer.frame();
    }

    return 0;
}
