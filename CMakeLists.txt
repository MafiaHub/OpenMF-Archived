cmake_minimum_required(VERSION 3.1.0)
project (OpenMF)
message(STATUS "Configuring OpenMF...")

find_package(OpenSceneGraph 3.3.4 REQUIRED osgDB osgViewer osgText osgGA osgParticle osgUtil osgFX)
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

set(USED_OSG_PLUGINS
    osgdb_bmp
    osgdb_dds
    osgdb_jpeg
    osgdb_osg
    osgdb_png
    osgdb_serializers_osg
    osgdb_tga)

add_subdirectory(game)
