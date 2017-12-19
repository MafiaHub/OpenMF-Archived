#ifndef OSG_UTILS_H
#define OSG_UTILS_H

namespace MFUtil
{

class MoveEarthSkyWithEyePointTransform: public osg::Transform
{ 
public: 
    virtual bool computeLocalToWorldMatrix (osg::Matrix & matrix, osg::NodeVisitor * nv) const
    { 
        osgUtil::CullVisitor * cv = dynamic_cast <osgUtil::CullVisitor*> (nv); 

        if (nv) 
        { 
            osg::Vec3 eyePointLocal = cv-> getEyeLocal (); 
            matrix.preMult (osg::Matrix::translate (eyePointLocal)); 
        } 

        return true; 
    }

    virtual bool computeWorldToLocalMatrix (osg :: Matrix & matrix, osg :: NodeVisitor * nv) const
    { 
        osgUtil::CullVisitor * cv = dynamic_cast <osgUtil::CullVisitor*> (nv); 

        if (nv)
        { 
            osg :: Vec3 eyePointLocal = cv-> getEyeLocal (); 
            matrix.postMult (osg :: Matrix :: translate (-eyePointLocal)); 
        }

        return true; 
    } 
};

}

#endif
