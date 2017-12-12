#include <iostream>
#include <fstream>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg_loader.hpp>

int main( int argc, char** argv )
{
    MFFormat::Loader loader;

    osgViewer::Viewer viewer;

    std::ifstream f;
    viewer.setSceneData( loader.load4ds(f) );
    return viewer.run();
}
