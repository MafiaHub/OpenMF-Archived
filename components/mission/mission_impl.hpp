#pragma once

#include "mission/mission.hpp"
#include "vfs/vfs.hpp"

#include <osg/Group>

// --- CLEANUP
#include <renderer/base_renderer.hpp>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <4ds/osg_4ds.hpp>
#include <scene2_bin/osg_scene2bin.hpp>
#include <cache_bin/osg_cachebin.hpp>
#include <check_bin/osg_checkbin.hpp>
#include <osg/Texture2D>
#include <osg/LightModel>
#include <utils/logger.hpp>
#include <osgGA/TrackballManipulator>
#include <loader_cache.hpp>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/Optimizer>
#include <osg/Fog>
#include <osgUtil/PrintVisitor>
#include <vfs/vfs.hpp>
#include <osg/MatrixTransform>
#include <fstream>
#include <utils/logger.hpp>
#include <vfs/vfs.hpp>
#include <loader_cache.hpp>
#include <utils/math.hpp>
// ---

#include "scene2_bin/parser_scene2bin.hpp"
#include "klz/parser_klz.hpp"
#include "renderer/osg_renderer.hpp"

namespace MFGame
{

    class Engine;

class MissionImpl: public Mission
{
public:
    MissionImpl(std::string missionName, MFGame::Engine *engine);
    virtual ~MissionImpl() override;

    virtual bool load() override;
    virtual bool unload() override;
    virtual bool importFile() override;
    virtual bool exportFile() override;

protected:
    MFFormat::DataFormat4DS mSceneModel;
    MFFormat::DataFormatScene2BIN mSceneData;
    MFFormat::DataFormatCacheBIN mCacheData;
    MFFormat::DataFormatTreeKLZ mStaticColsData;

    MFFormat::OSGLoader::NodeMap mNodeMap;

private:
    MFFile::FileSystem *mFileSystem;
    MFRender::OSGRenderer *mRenderer;
    MFGame::Engine *mEngine;

    void createMissionEntities();

};

}