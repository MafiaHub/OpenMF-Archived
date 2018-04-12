#pragma once

#include "mission/mission.hpp"
#include "vfs/vfs.hpp"

#include <osg/Group>

#include <scene2_bin/osg_scene2bin.hpp>
#include <cache_bin/osg_cachebin.hpp>
#include <check_bin/osg_checkbin.hpp>
#include <vfs/vfs.hpp>
#include <utils/math.hpp>

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

    MFFormat::DataFormatScene2BIN *getSceneData() { return &mSceneData; }

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