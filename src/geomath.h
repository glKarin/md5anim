//
// Created by Administrator on 2024/2/2.
//

#ifndef _GEOMATH_H
#define _GEOMATH_H

#include "str.h"

#define PI  3.14159265358979323846
#define M_DEG2RAD  (PI / 180.0)
#define M_RAD2DEG		 (180.0 / PI)
#ifdef FLT_EPSILON
#undef FLT_EPSILON
#endif
#define FLT_EPSILON		 1.192092896e-07
#define DEG2RAD(a) ( (a) * M_DEG2RAD )
#define RAD2DEG(a) ( (a) * M_RAD2DEG )

#include "text.h"

class idMath
{
public:
    static void SinCos( double a, double &s, double &c );
    static idStr ToString(double x);
    static idStr ToString(int x);
};

class idVec3
{
public:
    double x = 0;
    double y = 0;
    double z = 0;

    idVec3() = default;

    idVec3(double x, double y, double z)
    : x(x),
    y(y),
    z(z)
    {}

    explicit idVec3(const char *str);

    void Set(double _x, double _y, double _z)
    {
        this->x = _x;
        this->y = _y;
        this->z = _z;
    }

    idVec3 & operator+=(const idVec3 &b);
    friend idVec3 operator+(const idVec3 &a, const idVec3 &b);
    idVec3 & operator*=(double d);

    void Restore(const idStr &text)
    {
        idStr p = text;
        x = idLexer::GetDouble(p, &p);
        y = idLexer::GetDouble(p, &p);
        z = idLexer::GetDouble(p, &p);
    }

    idStr ToString(int p = 10) const
    {
        idStr str;
        str += x;
        str += " ";
        str += y;
        str += " ";
        str += z;
        return str;
    }

    void Rotate(const idVec3 &r);
};

class idQuat;
class idAngles
{
public:
    double pitch = 0;
    double yaw = 0;
    double roll = 0;

    idQuat ToQuat() const;

    void ToVectors(idVec3 *forward, idVec3 *right = nullptr, idVec3 *up = nullptr) const;
    idVec3 ToForward() const;
};

class idMat3
{
public:
    double mat[3][3] = {
            {1, 0, 0},
            {0, 1, 0},
            {0, 0, 1},
    };

    idAngles ToAngles() const;
};

class idQuat
        {
public:
    double x = 0;
    double y = 0;
    double z = 0;
    double w = 0;

    idQuat() = default;

    idQuat(double x, double y, double z, double w)
            : x(x),
              y(y),
              z(z),
              w(w)
    { }

    void Set(double x, double y, double z);
    double CalcW() const;
    idMat3 ToMat3() const;
    idAngles ToAngles() const {
        return ToMat3().ToAngles();
    }
    idVec3 ToForward() const {
        return ToAngles().ToForward();
    }
    void ToVectors(idVec3 *forward, idVec3 *right = nullptr, idVec3 *up = nullptr) const {
        ToAngles().ToVectors(forward, right, up);
    }

    idStr ToString() const;
};

#endif //_GEOMATH_H
