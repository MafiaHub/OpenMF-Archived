#ifndef OPENMW_COMPONENTS_SCENEUTIL_LIGHTMANAGER_H
#define OPENMW_COMPONENTS_SCENEUTIL_LIGHTMANAGER_H

// drummy: I've taken this file from OpenMW and edited it to suit OpenMF.

#include <set>
#include <osg/Light>
#include <osg/Group>
#include <osg/NodeVisitor>
#include <osg/observer_ptr>

#include <osgUtil/CullVisitor>

namespace osgUtil
{
    class CullVisitor;
}

namespace SceneUtil
{
    void transformBoundingSphere (const osg::Matrixf& matrix, osg::BoundingSphere& bsphere)
    {
        osg::BoundingSphere::vec_type xdash = bsphere._center;
        xdash.x() += bsphere._radius;
        xdash = xdash*matrix;

        osg::BoundingSphere::vec_type ydash = bsphere._center;
        ydash.y() += bsphere._radius;
        ydash = ydash*matrix;

        osg::BoundingSphere::vec_type zdash = bsphere._center;
        zdash.z() += bsphere._radius;
        zdash = zdash*matrix;

        bsphere._center = bsphere._center*matrix;

        xdash -= bsphere._center;
        osg::BoundingSphere::value_type sqrlen_xdash = xdash.length2();

        ydash -= bsphere._center;
        osg::BoundingSphere::value_type sqrlen_ydash = ydash.length2();

        zdash -= bsphere._center;
        osg::BoundingSphere::value_type sqrlen_zdash = zdash.length2();

        bsphere._radius = sqrlen_xdash;
        if (bsphere._radius<sqrlen_ydash) bsphere._radius = sqrlen_ydash;
        if (bsphere._radius<sqrlen_zdash) bsphere._radius = sqrlen_zdash;
        bsphere._radius = sqrtf(bsphere._radius);
    }

    /// LightSource managed by a LightManager.
    /// @par Typically used for point lights. Spot lights are not supported yet. Directional lights affect the whole scene
    ///     so do not need to be managed by a LightManager - so for directional lights use a plain osg::LightSource instead.
    /// @note LightSources must be decorated by a LightManager node in order to have an effect. Typical use would
    ///     be one LightManager as the root of the scene graph.
    /// @note One needs to attach LightListCallback's to the scene to have objects receive lighting from LightSources.
    ///     See the documentation of LightListCallback for more information.
    /// @note The position of the contained osg::Light is automatically updated based on the LightSource's world position.
    class LightSource : public osg::Node
    {
        // double buffered osg::Light's, since one of them may be in use by the draw thread at any given time
        osg::ref_ptr<osg::Light> mLight[2];

        // LightSource will affect objects within this radius
        float mRadius;

        int mId;

    public:

        META_Node(SceneUtil, LightSource)

        LightSource();

        LightSource(const LightSource& copy, const osg::CopyOp& copyop);

        float getRadius() const
        {
            return mRadius;
        }

        /// The LightSource will affect objects within this radius.
        void setRadius(float radius)
        {
            mRadius = radius;
        }

        /// Get the osg::Light safe for modification in the given frame.
        /// @par May be used externally to animate the light's color/attenuation properties,
        /// and is used internally to synchronize the light's position with the position of the LightSource.
        osg::Light* getLight(unsigned int frame)
        {
            return mLight[frame % 2];
        }

        /// @warning It is recommended not to replace an existing osg::Light, because there might still be
        /// references to it in the light StateSet cache that are associated with this LightSource's ID.
        /// These references will stay valid due to ref_ptr but will point to the old object.
        /// @warning Do not modify the \a light after you've called this function.
        void setLight(osg::Light* light)
        {
            mLight[0] = light;
            mLight[1] = new osg::Light(*light);
        }

        /// Get the unique ID for this light source.
        int getId() const
        {
            return mId;
        }
    };

    /// @brief Decorator node implementing the rendering of any number of LightSources that can be anywhere in the subgraph.
    class LightManager : public osg::Group
    {
    public:

        META_Node(SceneUtil, LightManager)

        LightManager();

        LightManager(const LightManager& copy, const osg::CopyOp& copyop);

        /// @param mask This mask is compared with the current Camera's cull mask to determine if lighting is desired.
        /// By default, it's ~0u i.e. always on.
        /// If you have some views that do not require lighting, then set the Camera's cull mask to not include
        /// the lightingMask for a much faster cull and rendering.
        void setLightingMask (unsigned int mask);

        unsigned int getLightingMask() const;

        /// Set the first light index that should be used by this manager, typically the number of directional lights in the scene.
        void setStartLight(int start);

        int getStartLight() const;

        /// Internal use only, called automatically by the LightManager's UpdateCallback
        void update();

        /// Internal use only, called automatically by the LightSource's UpdateCallback
        void addLight(LightSource* lightSource, const osg::Matrixf& worldMat, unsigned int frameNum);

        struct LightSourceTransform
        {
            LightSource* mLightSource;
            osg::Matrixf mWorldMatrix;
        };

        const std::vector<LightSourceTransform>& getLights() const;

        struct LightSourceViewBound
        {
            LightSource* mLightSource;
            osg::BoundingSphere mViewBound;
        };

        const std::vector<LightSourceViewBound>& getLightsInViewSpace(osg::Camera* camera, const osg::RefMatrix* viewMatrix);

        typedef std::vector<const LightSourceViewBound*> LightList;

        osg::ref_ptr<osg::StateSet> getLightListStateSet(const LightList& lightList, unsigned int frameNum);

    private:
        // Lights collected from the scene graph. Only valid during the cull traversal.
        std::vector<LightSourceTransform> mLights;

        typedef std::vector<LightSourceViewBound> LightSourceViewBoundCollection;
        std::map<osg::observer_ptr<osg::Camera>, LightSourceViewBoundCollection> mLightsInViewSpace;

        // < Light list hash , StateSet >
        typedef std::map<size_t, osg::ref_ptr<osg::StateSet> > LightStateSetMap;
        LightStateSetMap mStateSetCache[2];

        int mStartLight;

        unsigned int mLightingMask;
    };

    /// To receive lighting, objects must be decorated by a LightListCallback. Light list callbacks must be added via
    /// node->addCullCallback(new LightListCallback). Once a light list callback is added to a node, that node and all
    /// its child nodes can receive lighting.
    /// @par The placement of these LightListCallbacks affects the granularity of light lists. Having too fine grained
    /// light lists can result in degraded performance. Too coarse grained light lists can result in lights no longer
    /// rendering when the size of a light list exceeds the OpenGL limit on the number of concurrent lights (8). A good
    /// starting point is to attach a LightListCallback to each game object's base node.
    /// @note Not thread safe for CullThreadPerCamera threading mode.
    /// @note Due to lack of OSG support, the callback does not work on Drawables.
    class LightListCallback : public osg::NodeCallback
    {
    public:
        LightListCallback()
            : mLightManager(NULL)
            , mLastFrameNumber(0)
        {}
        LightListCallback(const LightListCallback& copy, const osg::CopyOp& copyop)
            : osg::Object(copy, copyop), osg::NodeCallback(copy, copyop)
            , mLightManager(copy.mLightManager)
            , mLastFrameNumber(0)
            , mIgnoredLightSources(copy.mIgnoredLightSources)
        {}

        META_Object(SceneUtil, LightListCallback)

        void operator()(osg::Node* node, osg::NodeVisitor* nv);

        bool pushLightState(osg::Node* node, osgUtil::CullVisitor* nv);

        std::set<SceneUtil::LightSource*>& getIgnoredLightSources() { return mIgnoredLightSources; }

    private:
        LightManager* mLightManager;
        unsigned int mLastFrameNumber;
        LightManager::LightList mLightList;
        std::set<SceneUtil::LightSource*> mIgnoredLightSources;
    };

    class LightStateCache
    {
    public:
        osg::Light* lastAppliedLight[8];
    };

    LightStateCache* getLightStateCache(unsigned int contextid)
    {
        static std::vector<LightStateCache> cacheVector;
        if (cacheVector.size() < contextid+1)
            cacheVector.resize(contextid+1);
        return &cacheVector[contextid];
    }

    // Resets the modelview matrix to just the view matrix before applying lights.
    class LightStateAttribute : public osg::StateAttribute
    {
    public:
        LightStateAttribute() : mIndex(0) {}
        LightStateAttribute(unsigned int index, const std::vector<osg::ref_ptr<osg::Light> >& lights) : mIndex(index), mLights(lights) {}

        LightStateAttribute(const LightStateAttribute& copy,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
            : osg::StateAttribute(copy,copyop), mIndex(copy.mIndex), mLights(copy.mLights) {}

        unsigned int getMember() const
        {
            return mIndex;
        }

        virtual bool getModeUsage(ModeUsage & usage) const
        {
            for (unsigned int i=0; i<mLights.size(); ++i)
                usage.usesMode(GL_LIGHT0 + mIndex + i);
            return true;
        }

        virtual int compare(const StateAttribute &sa) const
        {
            throw std::runtime_error("LightStateAttribute::compare: unimplemented");
        }

        META_StateAttribute(NifOsg, LightStateAttribute, osg::StateAttribute::LIGHT)

        virtual void apply(osg::State& state) const
        {
            if (mLights.empty())
                return;
            osg::Matrix modelViewMatrix = state.getModelViewMatrix();

            state.applyModelViewMatrix(state.getInitialViewMatrix());

            LightStateCache* cache = getLightStateCache(state.getContextID());

            for (unsigned int i=0; i<mLights.size(); ++i)
            {
                osg::Light* current = cache->lastAppliedLight[i+mIndex];
                if (current != mLights[i].get())
                {
                    applyLight((GLenum)((int)GL_LIGHT0 + i + mIndex), mLights[i].get());
                    cache->lastAppliedLight[i+mIndex] = mLights[i].get();
                }
            }

            state.applyModelViewMatrix(modelViewMatrix);
        }

        void applyLight(GLenum lightNum, const osg::Light* light) const
        {
            glLightfv( lightNum, GL_AMBIENT,               light->getAmbient().ptr() );
            glLightfv( lightNum, GL_DIFFUSE,               light->getDiffuse().ptr() );
            glLightfv( lightNum, GL_SPECULAR,              light->getSpecular().ptr() );
            glLightfv( lightNum, GL_POSITION,              light->getPosition().ptr() );
            // TODO: enable this once spot lights are supported
            // need to transform SPOT_DIRECTION by the world matrix?
            //glLightfv( lightNum, GL_SPOT_DIRECTION,        light->getDirection().ptr() );
            //glLightf ( lightNum, GL_SPOT_EXPONENT,         light->getSpotExponent() );
            //glLightf ( lightNum, GL_SPOT_CUTOFF,           light->getSpotCutoff() );
            glLightf ( lightNum, GL_CONSTANT_ATTENUATION,  light->getConstantAttenuation() );
            glLightf ( lightNum, GL_LINEAR_ATTENUATION,    light->getLinearAttenuation() );
            glLightf ( lightNum, GL_QUADRATIC_ATTENUATION, light->getQuadraticAttenuation() );
        }

    private:
        unsigned int mIndex;

        std::vector<osg::ref_ptr<osg::Light> > mLights;
    };

    LightManager* findLightManager(const osg::NodePath& path)
    {
        for (unsigned int i=0;i<path.size(); ++i)
        {
            if (LightManager* lightManager = dynamic_cast<LightManager*>(path[i]))
                return lightManager;
        }
        return NULL;
    }

    // Set on a LightSource. Adds the light source to its light manager for the current frame.
    // This allows us to keep track of the current lights in the scene graph without tying creation & destruction to the manager.
    class CollectLightCallback : public osg::NodeCallback
    {
    public:
        CollectLightCallback()
            : mLightManager(0) { }

        CollectLightCallback(const CollectLightCallback& copy, const osg::CopyOp& copyop)
            : osg::NodeCallback(copy, copyop)
            , mLightManager(0) { }

        META_Object(SceneUtil, SceneUtil::CollectLightCallback)

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
        {
            if (!mLightManager)
            {
                mLightManager = findLightManager(nv->getNodePath());

                if (!mLightManager)
                    throw std::runtime_error("can't find parent LightManager");
            }

            mLightManager->addLight(static_cast<LightSource*>(node), osg::computeLocalToWorld(nv->getNodePath()), nv->getTraversalNumber());

            traverse(node, nv);
        }

    private:
        LightManager* mLightManager;
    };

    // Set on a LightManager. Clears the data from the previous frame.
    class LightManagerUpdateCallback : public osg::NodeCallback
    {
    public:
        LightManagerUpdateCallback()
            { }

        LightManagerUpdateCallback(const LightManagerUpdateCallback& copy, const osg::CopyOp& copyop)
            : osg::NodeCallback(copy, copyop)
            { }

        META_Object(SceneUtil, LightManagerUpdateCallback)

        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
        {
            LightManager* lightManager = static_cast<LightManager*>(node);
            lightManager->update();

            traverse(node, nv);
        }
    };

    LightManager::LightManager()
        : mStartLight(0)
        , mLightingMask(~0u)
    {
        setUpdateCallback(new LightManagerUpdateCallback);
    }

    LightManager::LightManager(const LightManager &copy, const osg::CopyOp &copyop)
        : osg::Group(copy, copyop)
        , mStartLight(copy.mStartLight)
        , mLightingMask(copy.mLightingMask)
    {

    }

    void LightManager::setLightingMask(unsigned int mask)
    {
        mLightingMask = mask;
    }

    unsigned int LightManager::getLightingMask() const
    {
        return mLightingMask;
    }

    void LightManager::update()
    {
        mLights.clear();
        mLightsInViewSpace.clear();

        // do an occasional cleanup for orphaned lights
        for (int i=0; i<2; ++i)
        {
            if (mStateSetCache[i].size() > 5000)
                mStateSetCache[i].clear();
        }
    }

    void LightManager::addLight(LightSource* lightSource, const osg::Matrixf& worldMat, unsigned int frameNum)
    {
        LightSourceTransform l;
        l.mLightSource = lightSource;
        l.mWorldMatrix = worldMat;
        lightSource->getLight(frameNum)->setPosition(osg::Vec4f(worldMat.getTrans().x(),
                                                        worldMat.getTrans().y(),
                                                        worldMat.getTrans().z(), 1.f));
        mLights.push_back(l);
    }

    /* similar to the boost::hash_combine */
    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }

    osg::ref_ptr<osg::StateSet> LightManager::getLightListStateSet(const LightList &lightList, unsigned int frameNum)
    {

        // possible optimization: return a StateSet containing all requested lights plus some extra lights (if a suitable one exists)
        size_t hash = 0;
        for (unsigned int i=0; i<lightList.size();++i)
            hash_combine(hash, lightList[i]->mLightSource->getId());

        LightStateSetMap& stateSetCache = mStateSetCache[frameNum%2];

        LightStateSetMap::iterator found = stateSetCache.find(hash);
        if (found != stateSetCache.end())
            return found->second;
        else
        {
            osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
            std::vector<osg::ref_ptr<osg::Light> > lights;
            for (unsigned int i=0; i<lightList.size();++i)
            {
                lights.push_back(lightList[i]->mLightSource->getLight(frameNum));
            }

            // the first light state attribute handles the actual state setting for all lights
            // it's best to batch these up so that we don't need to touch the modelView matrix more than necessary
            osg::ref_ptr<LightStateAttribute> attr = new LightStateAttribute(mStartLight, lights);
            // don't use setAttributeAndModes, that does not support light indices!
            stateset->setAttribute(attr, osg::StateAttribute::ON);
            stateset->setAssociatedModes(attr, osg::StateAttribute::ON);

            // need to push some dummy attributes to ensure proper state tracking
            // lights need to reset to their default when the StateSet is popped
            for (unsigned int i=1; i<lightList.size(); ++i)
            {
                osg::ref_ptr<LightStateAttribute> dummy = new LightStateAttribute(mStartLight+i, std::vector<osg::ref_ptr<osg::Light> >());
                stateset->setAttribute(dummy, osg::StateAttribute::ON);
            }

            stateSetCache.insert(std::make_pair(hash, stateset));
            return stateset;
        }
    }

    const std::vector<LightManager::LightSourceTransform>& LightManager::getLights() const
    {
        return mLights;
    }

    const std::vector<LightManager::LightSourceViewBound>& LightManager::getLightsInViewSpace(osg::Camera *camera, const osg::RefMatrix* viewMatrix)
    {
        osg::observer_ptr<osg::Camera> camPtr (camera);
        std::map<osg::observer_ptr<osg::Camera>, LightSourceViewBoundCollection>::iterator it = mLightsInViewSpace.find(camPtr);

        if (it == mLightsInViewSpace.end())
        {
            it = mLightsInViewSpace.insert(std::make_pair(camPtr, LightSourceViewBoundCollection())).first;

            for (std::vector<LightSourceTransform>::iterator lightIt = mLights.begin(); lightIt != mLights.end(); ++lightIt)
            {
                osg::Matrixf worldViewMat = lightIt->mWorldMatrix * (*viewMatrix);
                osg::BoundingSphere viewBound = osg::BoundingSphere(osg::Vec3f(0,0,0), lightIt->mLightSource->getRadius());
                transformBoundingSphere(worldViewMat, viewBound);

                LightSourceViewBound l;
                l.mLightSource = lightIt->mLightSource;
                l.mViewBound = viewBound;
                it->second.push_back(l);
            }
        }
        return it->second;
    }

    class DisableLight : public osg::StateAttribute
    {
    public:
        DisableLight() : mIndex(0) {}
        DisableLight(int index) : mIndex(index) {}

        DisableLight(const DisableLight& copy,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
            : osg::StateAttribute(copy,copyop), mIndex(copy.mIndex) {}

        virtual osg::Object* cloneType() const { return new DisableLight(mIndex); }
        virtual osg::Object* clone(const osg::CopyOp& copyop) const { return new DisableLight(*this,copyop); }
        virtual bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const DisableLight *>(obj)!=NULL; }
        virtual const char* libraryName() const { return "SceneUtil"; }
        virtual const char* className() const { return "DisableLight"; }
        virtual Type getType() const { return LIGHT; }

        unsigned int getMember() const
        {
            return mIndex;
        }

        virtual bool getModeUsage(ModeUsage & usage) const
        {
            usage.usesMode(GL_LIGHT0 + mIndex);
            return true;
        }

        virtual int compare(const StateAttribute &sa) const
        {
            throw std::runtime_error("DisableLight::compare: unimplemented");
        }

        virtual void apply(osg::State& state) const
        {
            int lightNum = GL_LIGHT0 + mIndex;
            glLightfv( lightNum, GL_AMBIENT,               mNull.ptr() );
            glLightfv( lightNum, GL_DIFFUSE,               mNull.ptr() );
            glLightfv( lightNum, GL_SPECULAR,              mNull.ptr() );

            LightStateCache* cache = getLightStateCache(state.getContextID());
            cache->lastAppliedLight[mIndex] = NULL;
        }

    private:
        unsigned int mIndex;
        osg::Vec4f mNull;
    };

    void LightManager::setStartLight(int start)
    {
        mStartLight = start;

        // Set default light state to zero
        // This is necessary because shaders don't respect glDisable(GL_LIGHTX) so in addition to disabling
        // we'll have to set a light state that has no visible effect
        for (int i=start; i<8; ++i)
        {
            osg::ref_ptr<DisableLight> defaultLight (new DisableLight(i));
            getOrCreateStateSet()->setAttributeAndModes(defaultLight, osg::StateAttribute::OFF);
        }
    }

    int LightManager::getStartLight() const
    {
        return mStartLight;
    }

    static int sLightId = 0;

    LightSource::LightSource()
        : mRadius(0.f)
    {
        setUpdateCallback(new CollectLightCallback);
        mId = sLightId++;
    }

    LightSource::LightSource(const LightSource &copy, const osg::CopyOp &copyop)
        : osg::Node(copy, copyop)
        , mRadius(copy.mRadius)
    {
        mId = sLightId++;

        for (int i=0; i<2; ++i)
            mLight[i] = new osg::Light(*copy.mLight[i].get(), copyop);
    }


    bool sortLights (const LightManager::LightSourceViewBound* left, const LightManager::LightSourceViewBound* right)
    {
        return left->mViewBound.center().length2() - left->mViewBound.radius2()*81 < right->mViewBound.center().length2() - right->mViewBound.radius2()*81;
    }

    void LightListCallback::operator()(osg::Node *node, osg::NodeVisitor *nv)
    {
        osgUtil::CullVisitor* cv = static_cast<osgUtil::CullVisitor*>(nv);

        bool pushedState = pushLightState(node, cv);
        traverse(node, nv);
        if (pushedState)
            cv->popStateSet();
    }

    bool LightListCallback::pushLightState(osg::Node *node, osgUtil::CullVisitor *cv)
    {
        if (!mLightManager)
        {
            mLightManager = findLightManager(cv->getNodePath());
            if (!mLightManager)
                return false;
        }

        if (!(cv->getCurrentCamera()->getCullMask() & mLightManager->getLightingMask()))
            return false;

        // Possible optimizations:
        // - cull list of lights by the camera frustum
        // - organize lights in a quad tree


        // update light list if necessary
        // makes sure we don't update it more than once per frame when rendering with multiple cameras
        if (mLastFrameNumber != cv->getTraversalNumber())
        {
            mLastFrameNumber = cv->getTraversalNumber();

            // Don't use Camera::getViewMatrix, that one might be relative to another camera!
            const osg::RefMatrix* viewMatrix = cv->getCurrentRenderStage()->getInitialViewMatrix();
            const std::vector<LightManager::LightSourceViewBound>& lights = mLightManager->getLightsInViewSpace(cv->getCurrentCamera(), viewMatrix);

            // get the node bounds in view space
            // NB do not node->getBound() * modelView, that would apply the node's transformation twice
            osg::BoundingSphere nodeBound;
            osg::Transform* transform = node->asTransform();
            if (transform)
            {
                for (unsigned int i=0; i<transform->getNumChildren(); ++i)
                    nodeBound.expandBy(transform->getChild(i)->getBound());
            }
            else
                nodeBound = node->getBound();
            osg::Matrixf mat = *cv->getModelViewMatrix();
            transformBoundingSphere(mat, nodeBound);

            mLightList.clear();
            for (unsigned int i=0; i<lights.size(); ++i)
            {
                const LightManager::LightSourceViewBound& l = lights[i];

                if (mIgnoredLightSources.count(l.mLightSource))
                    continue;

                if (l.mViewBound.intersects(nodeBound))
                    mLightList.push_back(&l);
            }
        }
        if (!mLightList.empty())
        {
            unsigned int maxLights = static_cast<unsigned int> (8 - mLightManager->getStartLight());

            osg::StateSet* stateset = NULL;

            if (mLightList.size() > maxLights)
            {
                // remove lights culled by this camera
                LightManager::LightList lightList = mLightList;
                for (LightManager::LightList::iterator it = lightList.begin(); it != lightList.end() && lightList.size() > maxLights; )
                {
                    osg::CullStack::CullingStack& stack = cv->getModelViewCullingStack();

                    osg::BoundingSphere bs = (*it)->mViewBound;
                    bs._radius = bs._radius*2;
                    osg::CullingSet& cullingSet = stack.front();
                    if (cullingSet.isCulled(bs))
                    {
                        it = lightList.erase(it);
                        continue;
                    }
                    else
                        ++it;
                }

                if (lightList.size() > maxLights)
                {
                    // sort by proximity to camera, then get rid of furthest away lights
                    std::sort(lightList.begin(), lightList.end(), sortLights);
                    while (lightList.size() > maxLights)
                        lightList.pop_back();
                }
                stateset = mLightManager->getLightListStateSet(lightList, cv->getTraversalNumber());
            }
            else
                stateset = mLightManager->getLightListStateSet(mLightList, cv->getTraversalNumber());


            cv->pushStateSet(stateset);
            return true;
        }
        return false;
    }

}

#endif
