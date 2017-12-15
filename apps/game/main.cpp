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

    std::ifstream f;

    MFformat::DataFormatDTA dta;
    std::ifstream dtaFile("../mafia/A5.dta",std::ios::in|std::ios::binary);

dta.setDecryptKeys(MFformat::DataFormatDTA::A5_KEYS);
dta.load(dtaFile);

std::cout << dta.getNumFiles() << std::endl;
std::cout << dta.getFileName(0) << std::endl;
std::cout << dta.getFileName(1) << std::endl;
std::cout << dta.getFileName(2) << std::endl;

/*
std::ofstream outF;
outF.open("out.wav",std::ios::out | std::ios::binary);


char *b;
unsigned int l;
dta.getFile(dtaFile,0,&b,l);

for (int i = 0; i < l; ++i)
    outF << ((char) b[i]);

//outF.write(b,l);

outF.flush();
outF.close();
free(b);
*/

    dtaFile.close();

return 0;

    osgViewer::Viewer viewer;
    viewer.setUpViewInWindow(30, 30, 800, 600);

    osg::ref_ptr<osg::Node> n = loader.load4ds(f);
    osg::ref_ptr<osg::Texture2D> t = new osg::Texture2D();
    osg::ref_ptr<osg::Image> i = osgDB::readImageFile("../resources/test.tga");
    t->setImage(i);
    n->getOrCreateStateSet()->setTextureAttributeAndModes(0,t.get());   
    viewer.setSceneData(n);
    return viewer.run();

}
