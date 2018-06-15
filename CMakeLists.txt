cmake_minimum_required(VERSION 3.1.0)
project(OpenMF)
message(STATUS "Configuring OpenMF...")

option(BUILD_GAME   "build game"                 ON)
option(BUILD_VIEWER "build model/mission viewer" ON)
option(BUILD_UTILS  "build format utils"         ON)
option(BUILD_TESTS  "build tests"                ON)
option(LTO_BUILD    "build with LTO"             OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# specify debug information format and disable optimization build.
# TODO get rid of it once we make Debug build work correctly.
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
  string(REGEX REPLACE "/O2" "/Od" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(LTO_BUILD)
  cmake_policy(SET CMP0069 NEW)
  include(CheckIPOSupported)
  check_ipo_supported()
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

set(OpenMF_CMAKE_DIR "${OpenMF_SOURCE_DIR}/cmake")

string(COMPARE NOTEQUAL ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} CMAKE_OUT_OF_SOURCE_BUILD)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

if(CMAKE_OUT_OF_SOURCE_BUILD)
    set(DEFAULT_RELATIVE_PATHS OFF)
else()
    set(DEFAULT_RELATIVE_PATHS ON)
endif()

option(CMAKE_USE_RELATIVE_PATHS "If true, cmake will use relative paths in makefiles and projects." ${DEFAULT_RELATIVE_PATHS})

if(WIN32)
    set(OSG_DIR CACHE PATH "Path where to find the OpenSceneGraph")
    if(NOT OSG_DIR)
        #message(FATAL_ERROR "Error: OpenSceneGraph not found.")
        set(WIN32_DEP_not_FOUND "ON")
    else(NOT OSG_DIR)
        set(OSG_DIR "${OSG_DIR};${OSG_DIR}/build")
    endif(NOT OSG_DIR)

    set(OSG_THIRD_PARTY_DIR CACHE PATH "Path where to find the osg third party dependencies")

    if(NOT OSG_THIRD_PARTY_DIR)
    #    message(FATAL_ERROR "Error: OpenSceneGraph 3rd Party Directory not found.")
        set(WIN32_DEP_not_FOUND "ON")
    endif(NOT OSG_THIRD_PARTY_DIR)
    
    set(BULLET_ROOT CACHE PATH "Path where to find Bullet3")

    if(NOT BULLET_ROOT)
        #message(FATAL_ERROR "Error: Bullet3 not found.")
        set(WIN32_DEP_not_FOUND "ON")
    else(NOT BULLET_ROOT)
        set(BULLET_ROOT "${BULLET_ROOT}")
        set(BULLET_INCLUDE_DIR "${BULLET_ROOT}/../src;")
    endif(NOT BULLET_ROOT)
    
    set(SDL2_ROOT CACHE PATH "Path where to find SDL2")

    if(NOT SDL2_ROOT)
        #message(FATAL_ERROR "Error: SDL2 not found")
        set(WIN32_DEP_not_FOUND "ON")
    else(NOT SDL2_ROOT)
        set(SDL2_ROOT "${SDL2_ROOT}")
        set(SDL2_INCLUDE_DIR "${SDL2_ROOT}/include;${SDL2_ROOT}/include/SDL2;")
        if(${CMAKE_SIZEOF_VOID_P} MATCHES 8)
            set(SDL2_LIBRARIES "${SDL2_ROOT}/lib/x64/SDL2.lib;${SDL2_ROOT}/lib/x64/SDL2main.lib;")
        else(${CMAKE_SIZEOF_VOID_P} MATCHES 8)
            set(SDL2_LIBRARIES "${SDL2_ROOT}/lib/x86/SDL2.lib;${SDL2_ROOT}/lib/x86/SDL2main.lib;") 
        endif(${CMAKE_SIZEOF_VOID_P} MATCHES 8)
    endif(NOT SDL2_ROOT)

    if(WIN32_DEP_not_FOUND)
        message(FATAL_ERROR "Error: Some of your dependencies could not be found.")
    endif(WIN32_DEP_not_FOUND)
endif(WIN32)

if(CMAKE_CONFIGURATION_TYPES)
   set(CMAKE_CONFIGURATION_TYPES Debug Release)
   set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
     "Reset the configurations to what we need"
     FORCE)
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH    ${PROJECT_BINARY_DIR}/lib)

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D \"WIN32_LEAN_AND_MEAN\"")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D \"NOMINMAX\"")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")

    if(MSVC80)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wp64")
    endif(MSVC80)

    option(MULTI_PROCESSOR_COMPILATION "Use multiple processors when compiling" ON)

    if(MULTI_PROCESSOR_COMPILATION)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    endif(MULTI_PROCESSOR_COMPILATION)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

    if (WARNINGS_AS_ERRORS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
    endif (WARNINGS_AS_ERRORS)
endif(WIN32)

find_package(OpenSceneGraph 3.4.1 REQUIRED osgDB osgViewer osgText osgGA osgParticle osgUtil osgFX)

if(NOT WIN32)
    find_package(SDL2 REQUIRED)
endif(NOT WIN32)

find_package(Threads)
find_package(Bullet 2.88 REQUIRED BulletCommon BulletDynamics BulletCollision LinearMath)

include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS} ${BULLET_INCLUDE_DIRS} ${SDL2_INCLUDE_DIR} "/usr/include/bullet")

if(NOT WIN32)
    LINK_DIRECTORIES(${SDL2_LIBRARY_DIRS})
endif(NOT WIN32)

set(USED_OSG_PLUGINS
    osgdb_bmp
    osgdb_dds
    osgdb_jpeg
    osgdb_osg
    osgdb_png
    osgdb_serializers_osg
    osgdb_tga)

include_directories(components)
include_directories(extern)
include_directories(tests)

if(NOT WIN32)
    string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
endif(NOT WIN32)

file(GLOB_RECURSE COMPONENT_SOURCES 
    "components/*.hpp"
    "extern/*.hpp"

    "components/parser_*.cpp"
    "components/utils/openmf.cpp"
    "components/utils/logger.cpp"
    "components/utils/bmp_analyser.cpp"
    "components/vfs/*.cpp"
    "components/dta/key_extractor.cpp")

file(GLOB_RECURSE GAME_COMPONENT_SOURCES 
    "components/osg_*.cpp" 
    "components/bullet_*.cpp" 
    "components/controllers/*.cpp" 
    "components/physics/*.cpp"
    "components/input/*.cpp"
    "components/renderer/*.cpp"
    "components/entity/*.cpp"
    "components/engine/*.cpp"
    "components/mission/*.cpp"
    "components/utils/osg.cpp"
    "components/utils/bullet.cpp"
    "components/base_loader.cpp"
    )

set (GAME_COMPONENT_SOURCES
    "${GAME_COMPONENT_SOURCES}"
    "extern/sdl_graphics_window.cpp"
    "extern/GL/gl3w.c"
    "extern/imgui/imgui.cpp"
    "extern/imgui/imgui_draw.cpp"
    "extern/imgui/imgui_demo.cpp"
    "extern/imgui/ImGuiHandler.cpp")


set(THIRD_PARTY_LIBS
    ${OPENSCENEGRAPH_LIBRARIES}
    ${OPENTHREADS_LIBRARIES}
    ${OSGPARTICLE_LIBRARIES}
    ${OSGUTIL_LIBRARIES}
    ${OSGDB_LIBRARIES}
    ${OSGVIEWER_LIBRARIES}
    ${OSGGA_LIBRARIES}
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT})

if(WIN32)
    set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} opengl32)
elseif(APPLE)
    set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} "-framework OpenGL -framework CoreFoundation")
else(WIN32)
    set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} GL)
endif(WIN32)

set(GAME_THIRD_PARTY_LIBS 
    ${THIRD_PARTY_LIBS}
    ${BULLET_LIBRARIES}
    ${SDL2_LIBRARIES})

add_library(components OBJECT ${COMPONENT_SOURCES})
add_library(game_components OBJECT ${COMPONENT_SOURCES} ${GAME_COMPONENT_SOURCES})

if(BUILD_VIEWER)
    add_executable        ( viewer     apps/viewer/main.cpp              $<TARGET_OBJECTS:game_components> )
    target_link_libraries ( viewer                                       ${GAME_THIRD_PARTY_LIBS}  )
endif(BUILD_VIEWER)

if(BUILD_GAME)
    add_executable        ( game       apps/game/main.cpp                $<TARGET_OBJECTS:game_components> )
    target_link_libraries ( game                                         ${GAME_THIRD_PARTY_LIBS}  )
endif(BUILD_GAME)

if(BUILD_UTILS)
    add_executable        ( dta        apps/format_utils/dta.cpp         $<TARGET_OBJECTS:components> )
    target_link_libraries ( dta                                          ${THIRD_PARTY_LIBS}  )

    add_executable        ( cache_bin  apps/format_utils/cache_bin.cpp   $<TARGET_OBJECTS:components> )
    target_link_libraries ( cache_bin                                    ${THIRD_PARTY_LIBS}  )

    add_executable        ( check_bin  apps/format_utils/check_bin.cpp   $<TARGET_OBJECTS:components> )
    target_link_libraries ( check_bin                                    ${THIRD_PARTY_LIBS}  )

    add_executable        ( effects_bin   apps/format_utils/effects_bin.cpp   $<TARGET_OBJECTS:components> )
    target_link_libraries ( effects_bin                                       ${THIRD_PARTY_LIBS}  )

    add_executable        ( load_def   apps/format_utils/load_def.cpp    $<TARGET_OBJECTS:components> )
    target_link_libraries ( load_def                                     ${THIRD_PARTY_LIBS}  )

    add_executable        ( mnu        apps/format_utils/mnu.cpp         $<TARGET_OBJECTS:components> )
    target_link_libraries ( mnu                                          ${THIRD_PARTY_LIBS}  )

    add_executable        ( tree_klz   apps/format_utils/tree_klz.cpp    $<TARGET_OBJECTS:components> )
    target_link_libraries ( tree_klz                                     ${THIRD_PARTY_LIBS}  )

    add_executable        ( textdb     apps/format_utils/textdb.cpp      $<TARGET_OBJECTS:components> )
    target_link_libraries ( textdb                                       ${THIRD_PARTY_LIBS}  )

    add_executable        ( road_bin   apps/format_utils/road_bin.cpp    $<TARGET_OBJECTS:components> )
    target_link_libraries ( road_bin                                     ${THIRD_PARTY_LIBS}  )

    add_executable        ( scene2_bin apps/format_utils/scene2_bin.cpp  $<TARGET_OBJECTS:components> )
    target_link_libraries ( scene2_bin                                   ${THIRD_PARTY_LIBS}  )
endif(BUILD_UTILS)

if(BUILD_TESTS)
    add_executable        ( test_suite tests/test_suite.cpp              $<TARGET_OBJECTS:game_components>)
    target_link_libraries ( test_suite                                   ${GAME_THIRD_PARTY_LIBS})

    add_executable        ( test_suite_local tests/test_suite_local.cpp  $<TARGET_OBJECTS:game_components>)
    target_link_libraries ( test_suite_local                             ${GAME_THIRD_PARTY_LIBS})
endif(BUILD_TESTS)
