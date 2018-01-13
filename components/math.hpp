#ifndef MF_MATH_HPP
#define MF_MATH_HPP

#include <sstream>
#include <math.h>

namespace MFMath
{

#pragma pack(push, 1)

typedef struct Vec3_s
{
    float x;
    float y;
    float z;

    Vec3_s(): x(0), y(0), z(0) {};
    Vec3_s(float initX, float initY, float initZ): x(initX), y(initY), z(initZ) {};
    struct Vec3_s operator+(struct Vec3_s v) { struct Vec3_s r; r.x = x + v.x; r.y = y + v.y; r.z = z + v.z; return r; };
    struct Vec3_s operator-(struct Vec3_s v) { struct Vec3_s r; r.x = x - v.x; r.y = y - v.y; r.z = z - v.z; return r; };
    struct Vec3_s operator*(float v)         { struct Vec3_s r; r.x = x * v, r.y = y * v; r.z = z * v; return r;       };
    inline std::string str()                 { return "[" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + "]"; };
} Vec3;

typedef struct Vec2_s
{
    float x;
    float y;

    Vec2_s(): x(0), y(0) {};
    inline std::string str()                 { return "[" + std::to_string(x) + ", " + std::to_string(y) + "]"; };
} Vec2;

typedef struct Quat_s
{
    float x;
    float y;
    float z;
    float w;

    Quat_s(): x(0), y(0), z(0), w(1) {};

    void fromMafia()
    {
        float x0 = x; float y0 = y; float z0 = z; float w0 = w;
        x = y0;
        y = z0;
        z = w0;
        w = -1 * x0;
    }

    inline std::string str() { return "[" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + "]"; };
} Quat;

typedef struct Mat4_s
{
    float a0, a1, a2, a3;
    float b0, b1, b2, b3;
    float c0, c1, c2, c3;
    float d0, d1, d2, d3;

    Mat4_s(): a0(1), a1(0), a2(0), a3(0),
              b0(0), b1(1), b2(0), b3(0),
              c0(0), c1(0), c2(1), c3(0),
              d0(0), d1(0), d2(0), d3(1) {};

    Mat4_s(const Mat4_s &copyFrom)
    {
        a0 = copyFrom.a0; a1 = copyFrom.a1; a2 = copyFrom.a2; a3 = copyFrom.a3;
        b0 = copyFrom.b0; b1 = copyFrom.b1; b2 = copyFrom.b2; b3 = copyFrom.b3;
        c0 = copyFrom.c0; c1 = copyFrom.c1; c2 = copyFrom.c2; c3 = copyFrom.c3;
        d0 = copyFrom.d0; d1 = copyFrom.d1; d2 = copyFrom.d2; d3 = copyFrom.d3;
    }

    inline std::string str()
    {
        std::stringstream sstream;

        sstream << a0 << "\t" << a1 << "\t" << a2 << "\t" << a3 << std::endl;
        sstream << b0 << "\t" << b1 << "\t" << b2 << "\t" << b3 << std::endl;
        sstream << c0 << "\t" << c1 << "\t" << c2 << "\t" << c3 << std::endl;
        sstream << d0 << "\t" << d1 << "\t" << d2 << "\t" << d3 << std::endl;
        
        return sstream.str();
    }

    Vec3 getTranslation()
    {
        Vec3 result;
        result.x = d0;
        result.y = d1;
        result.z = d2;
        return result;
    }

    Vec3 getScale()
    {
        Vec3 result;
        double v;

        v = a0 * a0 + a1 * a1 + a2 * a2;
        v = v < 0 ? 0 : v;
        result.x = sqrt(v);

        v = b0 * b0 + b1 * b1 + b2 * b2;
        v = v < 0 ? 0 : v;
        result.y = sqrt(v);

        v = c0 * c0 + c1 * c1 + c2 * c2;
        v = v < 0 ? 0 : v;
        result.z = sqrt(v);

        return result;
    }

    void separateRotation()
    {
        d0 = 0;
        d1 = 0;
        d2 = 0;
        a3 = 0;
        b3 = 0;
        c3 = 0;
        d3 = 1;

        Vec3 scale = getScale();

        scale.x = scale.x == 0 ? 0.000001 : scale.x;
        scale.y = scale.y == 0 ? 0.000001 : scale.y;
        scale.z = scale.z == 0 ? 0.000001 : scale.z;

        a0 /= scale.x;
        a1 /= scale.y;
        a2 /= scale.z;

        b0 /= scale.x;
        b1 /= scale.y;
        b2 /= scale.z;

        c0 /= scale.x;
        c1 /= scale.y;
        c2 /= scale.z;
    }

    Quat getRotation()
    {
        Quat result;
        Mat4_s helper(*this);

        helper.separateRotation();

        double v = 1 + a0 + b1 + c2;
        v = v < 0.0 ? 0.0 : v;

        result.w = sqrt(v) / 2.0;

        double divisor = (4 * result.w);
        divisor = divisor == 0 ? 0.00001 : divisor;

        result.x = (c1 - b2) / divisor;
        result.y = (a2 - c0) / divisor;
        result.z = (b0 - a1) / divisor;

        return result;
    }
} Mat4;

#pragma pack(pop)

}

#endif
