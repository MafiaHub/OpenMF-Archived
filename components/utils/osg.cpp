#include <utils/osg.hpp>
#include <osg/Material>

namespace MFUtil
{

void traverse(osg::NodeVisitor *v,osg::Node &n)
{
    if (n.asGroup())
        for (int i = 0; i < (int) n.asGroup()->getNumChildren(); ++i)
            n.asGroup()->getChild(i)->accept(*v);
}

void rotationToVectors(osg::Quat rotation, osg::Vec3f &forw, osg::Vec3f &right, osg::Vec3f &up)
{
    forw  = osg::Vec3f(0,1,0);
    right = osg::Vec3f(1,0,0);
    up    = osg::Vec3f(0,0,1);

    osg::Matrixd m = osg::Matrixd::rotate(rotation);

    forw  = m.preMult(forw);
    right = m.preMult(right);
    up    = m.preMult(up);
}

void quatToEuler(osg::Quat q, double &yaw, double &pitch, double &roll)
{
    double qx = q.x(); 
    double qy = q.y(); 
    double qz = q.z(); 
    double qw = q.w(); 

    double sqx = qx * qx; 
    double sqy = qy * qy; 
    double sqz = qz * qz; 
    double sqw = qw * qw; 

    double term1 = 2 * (qx*qy+qw*qz); 
    double term2 = sqw + sqx - sqy - sqz; 
    double term3 = -2 * (qx * qz - qw * qy); 
    double term4 = 2 * (qw * qx + qy * qz); 
    double term5 = sqw - sqx - sqy + sqz; 

    yaw = atan2(term1, term2); 
    pitch = atan2(term4, term5); 
    roll = asin(term3); 
}

osg::Quat eulerToQuat(double yaw, double pitch, double roll)
{
    osg::Matrixd mat;

    mat.preMultRotate( osg::Quat(yaw,osg::Vec3f(0,0,1)) );
    mat.preMultRotate( osg::Quat(pitch,osg::Vec3f(1,0,0)) );
    mat.preMultRotate( osg::Quat(roll,osg::Vec3f(0,1,0)) );

    return mat.getRotate(); 
}

SkyboxNode::SkyboxNode(): osg::MatrixTransform()
{
    setName("camera relative");

    // disable lights for backdrop sector:
    osg::ref_ptr<osg::Material> mat = new osg::Material;

    mat->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4f(1,1,1,1));
    mat->setColorMode(osg::Material::OFF);
    getOrCreateStateSet()->setAttributeAndModes(mat,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

    getOrCreateStateSet()->setMode(GL_FOG,osg::StateAttribute::OFF);
    getOrCreateStateSet()->setRenderBinDetails(-1,"RenderBin");                    // render before the scene
    getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);  // don't write to depth buffer

    // FIXME: disabling depth writing this way also disables depth test => bad (osg::ClearNode?)
}

bool SkyboxNode::computeLocalToWorldMatrix(osg::Matrix & matrix, osg::NodeVisitor * nv) const
{ 
    osgUtil::CullVisitor * cv = dynamic_cast <osgUtil::CullVisitor*> (nv); 

    if (nv) 
    { 
        osg::Vec3 eyePointLocal = cv-> getEyeLocal(); 
        matrix.preMult(osg::Matrix::translate(eyePointLocal));
        matrix.preMult(osg::Matrixd::scale(osg::Vec3(0.1,0.1,0.1)));   // FIXME: This is here to prevent view frustum culling, not nice.
    }

    return true; 
}

bool SkyboxNode::computeWorldToLocalMatrix(osg :: Matrix & matrix, osg :: NodeVisitor * nv) const
{ 
    osgUtil::CullVisitor * cv = dynamic_cast <osgUtil::CullVisitor*> (nv); 

    if (nv)
    { 
        osg :: Vec3 eyePointLocal = cv-> getEyeLocal (); 
        matrix.postMult (osg :: Matrix :: translate (-eyePointLocal)); 
    }

    return true; 
} 

osg::ref_ptr<osg::Image> addAlphaFromImage(osg::Image *img, osg::Image *alphaImg)
{
    osg::ref_ptr<osg::Image> dstImg = new osg::Image;

    dstImg->allocateImage(img->s(),img->t(),1,GL_RGBA,GL_FLOAT);

    for (int y = 0; y < dstImg->t(); ++y)
        for (int x = 0; x < dstImg->s(); ++x)
        {
            osg::Vec4f alphaPixel = alphaImg->getColor(x,y);
            osg::Vec4f srcPixel = img->getColor(x,y);

            dstImg->setColor(osg::Vec4f(srcPixel.x(),srcPixel.y(),srcPixel.z(),alphaPixel.x()),x,y);
        }

    return dstImg;
}

osg::ref_ptr<osg::Image> applyColorKey(osg::Image *img, osg::Vec3f color, float err)
{
    osg::ref_ptr<osg::Image> dstImg = new osg::Image;

    dstImg->allocateImage(img->s(),img->t(),1,GL_RGBA,GL_FLOAT);

    for (int y = 0; y < dstImg->t(); ++y)
        for (int x = 0; x < dstImg->s(); ++x)
        {
            osg::Vec4f p = img->getColor(x,y);

            float alpha =
                (std::abs(p.x() - color.x()) <= err && 
                 std::abs(p.y() - color.y()) <= err &&
                 std::abs(p.z() - color.z()) <= err) ? 0.0 : 1.0;

            dstImg->setColor(osg::Vec4f(p.x(),p.y(),p.z(),alpha),x,y);
        }

    return dstImg;
}

RobustIntersectionVisitor::RobustIntersectionVisitor(osgUtil::Intersector* intersector):
    osgUtil::IntersectionVisitor(intersector)
{
}

std::string toString(osg::Vec3f v)
{
    return "[" + doubleToStr(v.x()) + ", " + doubleToStr(v.y()) + ", " + doubleToStr(v.z()) + "]";
}

std::string toString(osg::Quat q)
{
    return "<" + doubleToStr(q.x()) + ", " + doubleToStr(q.y()) + ", " + doubleToStr(q.z()) + ", " + doubleToStr(q.w()) + ">";
}

std::string rotationToStr(osg::Quat q)
{
    double y,p,r;
    quatToEuler(q,y,p,r);

    y = y / (2 * osg::PI) * 360;
    p = p / (2 * osg::PI) * 360;
    r = r / (2 * osg::PI) * 360;

    return "{" + doubleToStr(y) + ", " + doubleToStr(p) + ", " + doubleToStr(r) + "}";
}

std::string matrixTransformToString(osg::Matrixd m, bool rotationInEuler)
{
    osg::Vec3f trans, scale;
    osg::Quat rot, so;

    m.decompose(trans,rot,scale,so);

    std::string rotationStr = rotationInEuler ? rotationToStr(rot) : toString(rot);

    return toString(trans) + " " + rotationStr + " " + toString(scale);
}

std::string charArrayToStr(char *array, unsigned int length)
{
    char *buffer = (char *) malloc(length + 1);
    memcpy(buffer,array,length);
    buffer[length] = 0;   // terminate the string
    std::string result = buffer;
    free(buffer);
    return result;
}

InfoStringVisitor::InfoStringVisitor(): NodeVisitor()
{
    mFirst = true;
}

void InfoStringVisitor::apply(osg::Node &n)
{
    mInfo += n.className() + std::string(" (") + n.getName() + std::string("), parents: " + std::to_string(n.getNumParents()));
}

void InfoStringVisitor::apply(osg::Group &g)
{
    apply(dynamic_cast<osg::Node&>(g));
    mInfo += ", children: " + std::to_string(g.getNumChildren());
}

void InfoStringVisitor::apply(osg::MatrixTransform &t)
{
    apply(dynamic_cast<osg::Group&>(t));
    mInfo += ", transform: " + matrixTransformToString(t.getMatrix(),true);
}

std::string makeInfoString(osg::Node *n)
{
    InfoStringVisitor v;
    n->accept(v);
    return v.mInfo;
}

void AssignUserDataVisitor::apply(osg::Node &n)
{
    n.getOrCreateUserDataContainer()->addUserObject(mData);
    MFUtil::traverse(this,n);
}

}
