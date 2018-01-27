#ifndef OSG_UTILS_H
#define OSG_UTILS_H

#include <fstream>
#include <osg/Transform>
#include <osg/MatrixTransform>
#include <osgUtil/IntersectionVisitor>
#include <osgGA/FirstPersonManipulator>
#include <math.h>
#include <utils.hpp>

namespace MFUtil
{

/*** Call accept(v) on all children of n. */

void traverse(osg::NodeVisitor *v,osg::Node &n);

void rotationToVectors(osg::Quat rotation, osg::Vec3f &forw, osg::Vec3f &right, osg::Vec3f &up);

/** Convert quaternion rotation to yaw, pitch, roll (Euler angles), in radians. */

void quatToEuler(osg::Quat q, double &yaw, double &pitch, double &roll);

/** Convert Euler rotation (yaw, pitch, roll) to quaternion, in radians. */

osg::Quat eulerToQuat(double yaw, double pitch, double roll);

class MoveEarthSkyWithEyePointTransform: public osg::Transform
{ 
public: 
    virtual bool computeLocalToWorldMatrix (osg::Matrix & matrix, osg::NodeVisitor * nv) const;
    virtual bool computeWorldToLocalMatrix (osg :: Matrix & matrix, osg :: NodeVisitor * nv) const;
};

osg::ref_ptr<osg::Image> addAlphaFromImage(osg::Image *img, osg::Image *alphaImg);
osg::ref_ptr<osg::Image> applyColorKey(osg::Image *img, osg::Vec3f color, float err=0.01);

class RobustIntersectionVisitor: public osgUtil::IntersectionVisitor
{
public:
    RobustIntersectionVisitor(osgUtil::Intersector* intersector=0);

    virtual void apply(osg::MatrixTransform &transform) override       { osgUtil::IntersectionVisitor::apply(transform); };
    virtual void apply(osg::Transform &transform) override             { return;                                         };
};

std::string toString(osg::Vec3f v);
std::string toString(osg::Quat q);
std::string rotationToStr(osg::Quat q);
std::string matrixTransformToString(osg::Matrixd m, bool rotationInEuler=false);
std::string charArrayToStr(char *array, unsigned int length);

class InfoStringVisitor: public osg::NodeVisitor
{
public:
    InfoStringVisitor();

    void apply(osg::Node &n);
    void apply(osg::Group &g);
    void apply(osg::MatrixTransform &t);

    std::string mInfo;
    bool mFirst;
};

std::string makeInfoString(osg::Node *n);

/**
  User data that can be assigned to OSG objects.
*/

class UserData: public osg::Object
{
public:
    UserData(): osg::Object() { strcpy(mLibraryName,"MFUtil"); strcpy(mClassName,"UserData"); };

    virtual osg::Object *cloneType() const override                 { return 0;            };
    virtual osg::Object *clone(const osg::CopyOp &) const override  { return 0;            };
    virtual const char *libraryName() const override                { return mLibraryName; };
    virtual const char *className() const override                  { return mClassName;   };

protected:
    char mLibraryName[255];
    char mClassName[255];
};

class UserIntData: public UserData
{
public:
    UserIntData(int value): UserData() { mValue = value; strcpy(mClassName,"UserIntData"); };

    int mValue;
};

/**
  Assigns user data to given node and all children;
*/

class AssignUserDataVisitor: public osg::NodeVisitor
{
public:
    AssignUserDataVisitor(UserData *data): osg::NodeVisitor() { mData = data; };
    virtual void apply(osg::Node &n) override;

protected:
    osg::ref_ptr<UserData> mData;
};

}

#endif
