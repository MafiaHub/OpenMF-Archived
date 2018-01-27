#include <utils/bullet.hpp>

namespace MFUtil
{

inline btVector3 mafiaVec3ToBullet(double x, double y, double z)
{
    return btVector3(x,z,y); 
}

btTransform mafiaMat4ToBullet(MFMath::Mat4 m)
{
    MFMath::Vec3 trans = m.getTranslation();
    m.separateRotation();
        
    btMatrix3x3 rotMat;
    rotMat.setValue(m[0][0],m[0][1],m[0][2],m[1][0],m[1][1],m[1][2],m[2][0],m[2][1],m[2][2]);

    btTransform result(rotMat,mafiaVec3ToBullet(trans.x,trans.y,trans.z));
   
    btQuaternion q = result.getRotation();
    result.setRotation(btQuaternion(q.x(),q.z(),q.y(),q.w()));   /* TODO: find out why Y and Z have to be switched here */
    
    // TODO: scale

    return result;
}

}
