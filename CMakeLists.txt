CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)
PROJECT(OpenMF)
MESSAGE(STATUS "Configuring OpenMF...")

OPTION(BUILD_GAME   "build game"                 ON)
OPTION(BUILD_VIEWER "build model/mission viewer" ON)
OPTION(BUILD_UTILS  "build format utils"         ON)
OPTION(BUILD_TESTS  "build tests"                ON)

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(OpenMF_CMAKE_DIR "${OpenMF_SOURCE_DIR}/cmake")

STRING(COMPARE NOTEQUAL ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} CMAKE_OUT_OF_SOURCE_BUILD)

IF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
ENDIF()

IF(CMAKE_OUT_OF_SOURCE_BUILD)
    SET(DEFAULT_RELATIVE_PATHS OFF)
ELSE()
    SET(DEFAULT_RELATIVE_PATHS ON)
ENDIF()

OPTION(CMAKE_USE_RELATIVE_PATHS "If true, cmake will use relative paths in makefiles and projects." ${DEFAULT_RELATIVE_PATHS})

IF (WIN32)
    SET(OSG_DIR CACHE PATH "Path where to find the OpenSceneGraph")
    IF(NOT OSG_DIR)
        #MESSAGE(FATAL_ERROR "Error: OpenSceneGraph not found.")
        SET(WIN32_DEP_NOT_FOUND "ON")
    ELSE(NOT OSG_DIR)
        SET(OSG_DIR "${OSG_DIR};${OSG_DIR}/build")
    ENDIF(NOT OSG_DIR)

    SET(OSG_THIRD_PARTY_DIR CACHE PATH "Path where to find the osg third party dependencies")
    IF(NOT OSG_THIRD_PARTY_DIR)
    #    MESSAGE(FATAL_ERROR "Error: OpenSceneGraph 3rd Party Directory not found.")
        SET(WIN32_DEP_NOT_FOUND "ON")
    ENDIF(NOT OSG_THIRD_PARTY_DIR)
    
    SET(BULLET_ROOT CACHE PATH "Path where to find Bullet3")
    IF(NOT BULLET_ROOT)
        #MESSAGE(FATAL_ERROR "Error: Bullet3 not found.")
        SET(WIN32_DEP_NOT_FOUND "ON")
    ELSE(NOT BULLET_ROOT)
        SET(BULLET_ROOT "${BULLET_ROOT}")
        SET(BULLET_INCLUDE_DIR "${BULLET_ROOT}/../src;")
    ENDIF(NOT BULLET_ROOT)
    
    SET(SDL2_ROOT CACHE PATH "Path where to find SDL2")
    IF(NOT SDL2_ROOT)
        #MESSAGE(FATAL_ERROR "Error: SDL2 not found")
        SET(WIN32_DEP_NOT_FOUND "ON")
    ELSE(NOT SDL2_ROOT)
        SET(SDL2_ROOT "${SDL2_ROOT}")
        SET(SDL2_INCLUDE_DIR "${SDL2_ROOT}/include;${SDL2_ROOT}/include/SDL2;")
        IF(${CMAKE_SIZEOF_VOID_P} MATCHES 8)
            SET(SDL2_LIBRARIES "${SDL2_ROOT}/lib/x64/SDL2.lib;${SDL2_ROOT}/lib/x64/SDL2main.lib;")
        ELSE(${CMAKE_SIZEOF_VOID_P} MATCHES 8)
            SET(SDL2_LIBRARIES "${SDL2_ROOT}/lib/x86/SDL2.lib;${SDL2_ROOT}/lib/x86/SDL2main.lib;") 
        ENDIF(${CMAKE_SIZEOF_VOID_P} MATCHES 8)
    ENDIF(NOT SDL2_ROOT)

    IF(WIN32_DEP_NOT_FOUND)
        MESSAGE(FATAL_ERROR "Error: Some of your dependencies could not be found.")
    ENDIF(WIN32_DEP_NOT_FOUND)
ENDIF(WIN32)

IF(CMAKE_CONFIGURATION_TYPES)
   SET(CMAKE_CONFIGURATION_TYPES Debug Release)
   set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
     "Reset the configurations to what we need"
     FORCE)
ENDIF()

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

IF(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D \"WIN32_LEAN_AND_MEAN\"")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D \"NOMINMAX\"")
    
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")
    
    IF(MSVC80)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wp64")
    ENDIF(MSVC80)
    
    OPTION(MULTI_PROCESSOR_COMPILATION "Use multiple processors when compiling" ON)

    IF(MULTI_PROCESSOR_COMPILATION)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    ENDIF(MULTI_PROCESSOR_COMPILATION)

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

    IF (WARNINGS_AS_ERRORS)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
    ENDIF (WARNINGS_AS_ERRORS)
ENDIF(WIN32)

FIND_PACKAGE(OpenSceneGraph 3.4.1 REQUIRED osgDB osgViewer osgText osgGA osgParticle osgUtil osgFX)

IF(NOT WIN32)
    FIND_PACKAGE(SDL2 REQUIRED)
ENDIF(NOT WIN32)

FIND_PACKAGE(Threads)
FIND_PACKAGE(Bullet 2.88 REQUIRED BulletCommon BulletDynamics BulletCollision LinearMath)

INCLUDE_DIRECTORIES(${OPENSCENEGRAPH_INCLUDE_DIRS} ${BULLET_INCLUDE_DIRS} ${SDL2_INCLUDE_DIR} "/usr/include/bullet")

IF(NOT WIN32)
    LINK_DIRECTORIES(${SDL2_LIBRARY_DIRS})
ENDIF(NOT WIN32)

SET(USED_OSG_PLUGINS
    osgdb_bmp
    osgdb_dds
    osgdb_jpeg
    osgdb_osg
    osgdb_png
    osgdb_serializers_osg
    osgdb_tga)

INCLUDE_DIRECTORIES(components)
INCLUDE_DIRECTORIES(extern)

IF(NOT WIN32)
    string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
ENDIF(NOT WIN32)

FILE(GLOB_RECURSE COMPONENT_SOURCES "components/*.cpp" "extern/sdl_graphics_window.cpp")
SET(THIRD_PARTY_LIBS
    ${OPENSCENEGRAPH_LIBRARIES}
    ${OPENTHREADS_LIBRARIES}
    ${OSGPARTICLE_LIBRARIES}
    ${BULLET_LIBRARIES}
    ${OSGUTIL_LIBRARIES}
    ${OSGDB_LIBRARIES}
    ${OSGVIEWER_LIBRARIES}
    ${OSGGA_LIBRARIES}
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    ${SDL2_LIBRARIES})

ADD_EXECUTABLE        ( viewer     apps/viewer/main.cpp              ${COMPONENT_SOURCES} )
TARGET_LINK_LIBRARIES ( viewer                                       ${THIRD_PARTY_LIBS}  )

ADD_EXECUTABLE        ( dta        apps/format_utils/dta.cpp         ${COMPONENT_SOURCES} )
TARGET_LINK_LIBRARIES ( dta                                          ${THIRD_PARTY_LIBS}  )

ADD_EXECUTABLE        ( cache_bin  apps/format_utils/cache_bin.cpp   ${COMPONENT_SOURCES} )
TARGET_LINK_LIBRARIES ( cache_bin                                    ${THIRD_PARTY_LIBS}  )

ADD_EXECUTABLE        ( check_bin  apps/format_utils/check_bin.cpp   ${COMPONENT_SOURCES} )
TARGET_LINK_LIBRARIES ( check_bin                                    ${THIRD_PARTY_LIBS}  )

ADD_EXECUTABLE        ( load_def   apps/format_utils/load_def.cpp    ${COMPONENT_SOURCES} )
TARGET_LINK_LIBRARIES ( load_def                                     ${THIRD_PARTY_LIBS}  )

ADD_EXECUTABLE        ( mnu        apps/format_utils/mnu.cpp         ${COMPONENT_SOURCES} )
TARGET_LINK_LIBRARIES ( mnu                                          ${THIRD_PARTY_LIBS}  )

ADD_EXECUTABLE        ( tree_klz   apps/format_utils/tree_klz.cpp    ${COMPONENT_SOURCES} )
TARGET_LINK_LIBRARIES ( tree_klz                                     ${THIRD_PARTY_LIBS}  )

ADD_EXECUTABLE        ( textdb     apps/format_utils/textdb.cpp      ${COMPONENT_SOURCES} )
TARGET_LINK_LIBRARIES ( textdb                                       ${THIRD_PARTY_LIBS}  )

ADD_EXECUTABLE        ( road_bin   apps/format_utils/road_bin.cpp    ${COMPONENT_SOURCES} )
TARGET_LINK_LIBRARIES ( road_bin                                     ${THIRD_PARTY_LIBS}  )

ADD_EXECUTABLE        ( scene2_bin apps/format_utils/scene2_bin.cpp  ${COMPONENT_SOURCES} )
TARGET_LINK_LIBRARIES ( scene2_bin                                   ${THIRD_PARTY_LIBS}  )
