#include <osg/Node>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <fstream>

namespace MFFormat
{

class Loader
{
public:
    osg::ref_ptr<osg::Node> load4ds(std::ifstream &srcFile);
};

osg::ref_ptr<osg::Node> Loader::load4ds(std::ifstream &srcFile)
{
    osg::ref_ptr<osg::ShapeDrawable> result = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3f(0,0,0),1));
    return result;
}

}
