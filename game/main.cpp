#include <iostream>
#include <fstream>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg_loader.hpp>
#include <osgDB/ReadFile>
#include <osg/Texture2D>

int main( int argc, char** argv )
{
    MFFormat::Loader loader;

    osgViewer::Viewer viewer;
    std::ifstream f;
    viewer.setUpViewInWindow(30, 30, 800, 600);

    osg::ref_ptr<osg::Node> n = loader.load4ds(f);
    osg::ref_ptr<osg::Texture2D> t = new osg::Texture2D();
    osg::ref_ptr<osg::Image> i = osgDB::readImageFile("../resources/test.tga");
    t->setImage(i);
    n->getOrCreateStateSet()->setTextureAttributeAndModes(0,t.get());
    
    viewer.setSceneData(n);
    return viewer.run();
}
