#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <algorithm>
#include <base_shard.hpp>
#include <osg/Node>
#include <osg/Group>
#include <osg/ref_ptr>
#include <osg/ValueObject>

namespace MFEntity
{
    
class Entity
{
public:
    Entity() 
    {
        mGroup  = new osg::Group();
        mTransform = new osg::Node();
        mGroup->addChild(mTransform);
    }
    
    ~Entity() {}

    // TODO(zaklaus): Decide on what the pipeline should pass to the entity.
    void onInit();
    void onInput();
    void onUpdate();
    void onRender();
    void onDebugDraw();

    void addChild(Entity *entity);
    void removeChild(Entity *entity);
    void assign(Shard *shard);
    void dispose(Shard *shard);

    osg::ref_ptr<osg::Node> getTransform() { return mTransform; }
    osg::ref_ptr<osg::Group> getGroup() { return mGroup; }
    void                    setTransform(osg::ref_ptr<osg::Node> transform);

private:
    osg::ref_ptr<osg::Group> mGroup;
    std::vector<Shard*> mShards;
    std::vector<Entity*> mChildren;
    Entity *mParent;
    osg::ref_ptr<osg::Node> mTransform;
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

void Entity::addChild(Entity *entity)
{
    mGroup->addChild(entity->getGroup());
    mChildren.push_back(entity);
}

void Entity::removeChild(Entity *entity)
{
    mGroup->removeChild(entity->getGroup());
    mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), entity), mChildren.end());
}

void Entity::assign(Shard *shard)
{
    shard->setOwner(this);
    mShards.push_back(shard);
}

void Entity::dispose(Shard *shard)
{
    mShards.erase(std::remove(mShards.begin(), mShards.end(), shard), mShards.end());
    delete shard;
}

void Entity::setTransform(osg::ref_ptr<osg::Node> transform)
{
    for (auto parent : mTransform->getParents())
    {
        parent->addChild(transform);
    }

    for (auto parent : mTransform->getParents())
    {
        parent->removeChild(mTransform);
    }

    // TODO(zaklaus): Is this all we need to do to get rid of the previous osg::Node?
    mTransform = transform;
}

}

#endif // ENTITY_H