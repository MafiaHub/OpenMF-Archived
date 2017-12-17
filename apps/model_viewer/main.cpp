#include <iostream>
#include <fstream>
#include <osg/Node>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg_loader.hpp>
#include <4ds/parser.hpp>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/LightModel>
#include <loggers/console.hpp>
#include <cxxopts.hpp>

int main(int argc, char** argv)
{
    cxxopts::Options options("viewer","3D viewer for Mafia 4DS files.");

    options.add_options()
        ("h,help","Display help and exit.")
        ("i,input","Specify input file name.",cxxopts::value<std::string>())
        ("t,texdir","Specify texture directory.",cxxopts::value<std::string>());

    options.parse_positional({"i","t"});
    options.positional_help("file [texdir]");
    auto arguments = options.parse(argc,argv);

    if (arguments.count("h") > 0)
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (arguments.count("i") < 1)
    {
        MFLogger::ConsoleLogger::fatal("Expected file.");
        std::cout << options.help() << std::endl;
        return 1;
    }

    std::string inputFile = arguments["i"].as<std::string>();

    std::ifstream f;
    f.open(inputFile);

    if (!f.is_open())
    {
        MFLogger::ConsoleLogger::fatal("Could not open file " + inputFile + ".");
        return 1;
    }

    MFFormat::Loader loader;
    osgViewer::Viewer viewer;

    if (arguments.count("t") > 0)
        loader.setTextureDir(arguments["t"].as<std::string>());

    osg::ref_ptr<osg::Node> n = loader.load4ds(f);

    f.close();

    if (arguments.count("t") < 1)    // apply test texture
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
