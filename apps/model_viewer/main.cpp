#include <iostream>
#include <fstream>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg_loader.hpp>
#include <format_parsers.hpp>
#include <osgDB/ReadFile>
#include <osg/Texture2D>

int main( int argc, char** argv )
{
    MFFormat::Loader loader;

    osgViewer::Viewer viewer;

    std::ifstream f;
    f.open("../mafia/MODELS/baliksena.4ds");

    osg::ref_ptr<osg::Node> n = loader.load4ds(f);
    osg::ref_ptr<osg::Texture2D> t = new osg::Texture2D();
    osg::ref_ptr<osg::Image> i = osgDB::readImageFile("../resources/test.tga");
    t->setImage(i);

    f.close();
    n->getOrCreateStateSet()->setTextureAttributeAndModes(0,t.get());   
    viewer.setSceneData(n);
    return viewer.run();
}
