/********************************

header-only debug library for OSG
WTFPL license
Miloslav "drummyfish" Ciz, 2017

usage:

to_str(what,[align])        convert to string, optional align
print(what,[align])         print as string, optional align
trans(mat)                  treat matrix mat as transform, e.g. print(trans(m))
rot(quat)                   treat quaternion as rotation, e.g. print(rot(q))
sep([len],[char])           print separator
print_scene(root,[depth])   print scene graph
print_parents(node)         print parents (node path)
world_trans(node,[levels])  get world transform matrix, optional maximum parents to ascend

********************************/

#ifndef OSGDEBUG_H
#define OSGDEBUG_H

#include <iostream>
#include <sstream>
#include <iomanip>

#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Group>
#include <osg/Node>

#define SSTR( x ) static_cast< std::ostringstream & >( ( std::ostringstream() << std::dec << std::setprecision(5) << std::setiosflags(std::ios_base::fixed) << x ) ).str()

class rot     // Euler angles in degrees rotation
{
public:
    rot() {};

    rot(osg::Quat q)
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

        yaw = atan2(term1, term2) / (2 * osg::PI) * 360; 
        pitch = atan2(term4, term5) / (2 * osg::PI) * 360; 
        roll = asin(term3) / (2 * osg::PI) * 360; 
    }

    double yaw;
    double pitch;
    double roll;
};

class trans   // transformation
{
public:
    trans(osg::Matrixd m)
    {
        translation = m.getTrans();
        scale = m.getScale();
        rotation = rot(m.getRotate());
    }

    osg::Vec3f translation;
    osg::Vec3f scale;
    rot rotation;
};

std::string to_str(osg::Vec2 v, int align=-1)
{
    std::string res = align >= 0 ?
        SSTR(std::setw(align) << v.x() << std::setw(align) << v.y()):
        SSTR(v.x() << " " << v.y() << " ");
    return "Vec2: " + res;
}

std::string to_str(osg::Vec3 v, int align=-1)
{
    std::string res = align >= 0 ?
        SSTR(std::setw(align) << v.x() << std::setw(align) << v.y() << std::setw(align) << v.z()):
        SSTR(v.x() << " " << v.y() << " " << v.z() << " ");
    return "Vec3: " + res;
}

std::string to_str(osg::Vec4 v, int align=-1)
{
    std::string res = align >= 0 ?
        SSTR(std::setw(align) << v.x() << std::setw(align) << v.y() << std::setw(align) << v.z() << std::setw(align) << v.w()):
        SSTR(v.x() << " " << v.y() << " " << v.z() << " " << v.w() << " ");
    return "Vec4: " + res;
}

std::string to_str(osg::Quat q, int align=-1)
{
    std::string res = align >= 0 ?
        SSTR(std::setw(align) << q.x() << std::setw(align) << q.y() << std::setw(align) << q.z() << std::setw(align) << q.w()):
        SSTR(q.x() << " " << q.y() << " " << q.z() << " " << q.w() << " ");
    return "Quat: " + res;
}

std::string to_str(osg::Matrixd m, int align=-1)
{
    std::string result;

    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
            result += align >= 0 ?
                SSTR(m(row,col)):
                SSTR(std::setw(align) << m(row,col) << " ");
 
        result += "\n";
    }     
       
    return "Mat4: " + result;
}

std::string to_str(rot r, int align=-1)
{
    return SSTR("(" << r.yaw << " " << r.pitch << " " << r.roll << ")");
}

std::string to_str(trans t, int align=-1)
{
    return "transform (TRS): " + to_str(t.translation,align) + ", " + to_str(t.rotation) + ", " + to_str(t.scale,align);
}

void sep(int len=20, char c='-')       // print separator
{
    for (int i = 0; i < len; ++i)
        std::cout << c;

    std::cout << std::endl;
}

template<typename T>
void print(T what, int align=-1)
{
    std::cout << to_str(what,align) << std::endl;
}

class toStringVisitor: public osg::NodeVisitor
{
public:
    std::string result_string;

    virtual void apply(osg::Node &n) override
    {
        result_string += SSTR(n.className() << " (" << n.getName() << ")");
    }

    virtual void apply(osg::MatrixTransform &n) override
    {
        apply(*n.asNode());
        result_string += SSTR(": " << to_str(trans(n.getMatrix())));
    }

    virtual void apply(osg::Group &n) override
    {
        // PositionAttitudeTransform gets treated as Group - bug in OSG?
        if (std::string("PositionAttitudeTransform").compare(n.className()) == 0)
          {
              osg::PositionAttitudeTransform *p = (osg::PositionAttitudeTransform *) &n;
              apply(*p);
          }
        else
          {
              apply(*n.asNode());
              result_string += SSTR(": " << n.getNumChildren() << " children");
          }
    }

    virtual void apply(osg::PositionAttitudeTransform &n) override
    {
        apply(*n.asNode());
        result_string += SSTR(": pos = " << to_str(n.getPosition())  << ", rot = " << to_str(rot(n.getAttitude())));
    }
};

std::string to_str(osg::Node *n, int align=-1)
{
    toStringVisitor v;
    n->accept(v);
    return v.result_string;
}

template<typename T>
std::string to_str(const T *n, int align=-1)
{
    toStringVisitor v;
    osg::ref_ptr<T> n2 = new T(*n);  // to keep const
    n2->accept(v);
    return v.result_string;
}

osg::Matrixd world_trans(const osg::Node *n, int max_levels=-1)
{
    osg::NodePathList path = n->getParentalNodePaths();    

    if (max_levels >= 0)
    {
        max_levels += 1;   // node itself
        unsigned int loops = path[0].size() - max_levels;

        for (unsigned int i = 0; !path[0].empty() && i < loops; ++i) 
           path[0].erase(path[0].begin()); 
    }

    return osg::computeLocalToWorld(path[0]);
}

void print_scene(osg::Node *root, int max_level=-1, int level=0)
{
    if (max_level >= 0 && level > max_level)
        return;

    if (level == 0)
        std::cout << "SCENE:" << std::endl;

    for (int i = 0; i < level + 1; ++i)
        std::cout << "  ";
    
    std::cout << to_str(root) << std::endl;
    osg::Group *g = root->asGroup();

    if (g)
    {
       for (unsigned int i = 0; i < g->getNumChildren(); ++i)
           print_scene(g->getChild(i),max_level,level + 1); 
    }
}

void print_parents(const osg::Node *start)
{
   std::cout << "PARENTS OF NODE:" << std::endl;

   osg::NodePathList path = start->getParentalNodePaths();

   for (unsigned int i = 0; i < path[0].size(); ++i)
   {
       for (unsigned int j = 0; j < i + 1; ++j)
           std::cout << "  ";

       std::cout << to_str(path[0][i]) << std::endl;
   }
}

template<typename T>
void print(const T *what, int align=-1)
{
    std::cout << to_str(what,align) << std::endl;
}

#endif
