#include <iostream>
#include <fstream>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg_loader.hpp>
#include <format_parser_4ds.hpp>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/LightModel>

void printHelp()
{
    std::cout << "4DS model viewer" << std::endl << std::endl;
    std::cout << "usage: viewer 4dsfile [texturedir]" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Error: expected filename." << std::endl;
        printHelp();
        return 1;
    }

    std::ifstream f;
    f.open(argv[1]);

    if (!f.is_open())
    {
        std::cerr << "Error: could not open file: " << argv[1] << "." << std::endl;
        printHelp();
        return 1;
    }

    MFFormat::Loader loader;
    osgViewer::Viewer viewer;

    if (argc > 2)
        loader.setTextureDir(argv[2]);

    osg::ref_ptr<osg::Node> n = loader.load4ds(f);

    f.close();

    if (argc <= 2)    // apply test texture
    {
        osg::ref_ptr<osg::Texture2D> t = new osg::Texture2D();
        osg::ref_ptr<osg::Image> i = osgDB::readImageFile("../resources/test.tga");
        t->setImage(i);
        t->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);
        t->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);
        n->getOrCreateStateSet()->setTextureAttributeAndModes(0,t.get());
    }
    
    viewer.setSceneData(n);

    return viewer.run();
}
