#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <base_shard.hpp>

namespace MFEntity
{
    
class Entity
{
public:
    Entity() {}
    Entity(Entity *parent) : mParent(parent) {}
    ~Entity() {}

    // TODO(zaklaus): Decide on what the pipeline should pass to the entity.
    void onInit();
    void onInput();
    void onUpdate();
    void onRender();
    void onDebugDraw();

    void addChild(Entity *child);
    void assign(Shard *shard);

private:
    std::vector<Entity*> mChildren;
    std::vector<Shard*> mShards;
    Entity *mParent;
};

void Entity::onInit()
{
    for (auto shard : mShards)
        shard->onInit();

    for (auto child : mChildren)
        child->onInit();
}

void Entity::onInput()
{
    for (auto shard : mShards)
        shard->onInput();

    for (auto child : mChildren)
        child->onInput();
}

void Entity::onUpdate()
{
    for (auto shard : mShards)
        shard->onUpdate();

    for (auto child : mChildren)
        child->onUpdate();
}

void Entity::onRender()
{
    for (auto shard : mShards)
        shard->onRender();

    for (auto child : mChildren)
        child->onRender();
}

void Entity::onDebugDraw()
{
    for (auto shard : mShards)
        shard->onDebugDraw();

    for (auto child : mChildren)
        child->onDebugDraw();
}

void Entity::addChild(Entity *child)
{
    mChildren.push_back(child);
}

void Entity::assign(Shard *shard)
{
    shard->setOwner(this);
    mShards.push_back(shard);
}

}

#endif // ENTITY_H