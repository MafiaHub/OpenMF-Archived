#include <iostream>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/Shape>
#include <osg/ShapeDrawable>

int main( int argc, char** argv )
{
    std::cout << "test" << std::endl;
    osgViewer::Viewer viewer;
    osg::ref_ptr<osg::ShapeDrawable> s = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3f(0,0,0),1));
    viewer.setSceneData(s);
    return viewer.run();
}
