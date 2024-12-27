#include "geomath.h"

#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;

/*    public static double DEG2RAD(double a)
    {
        return ( (a) * M_DEG2RAD );
    }
    public static double RAD2DEG(double a)
    {
        return ( (a) * M_RAD2DEG );
    }*/

void idMath::SinCos( double a, double &s, double &c )		// sine and cosine with 32 bits precision
{
    s = (double) sin( a );
    c = (double) cos( a );
}

idStr idMath::ToString(double x)
{
    stringstream os;
    char text[128];
    snprintf(text, sizeof(text) - 1, "%.10f", x);
    os << text;
    return os.str();
}

idStr idMath::ToString(int x)
{
    stringstream os;
    os << x;
    return os.str();
}

void idQuat::Set(double _x, double _y, double _z)
{
    this->x = (double) _x;
    this->y = (double) _y;
    this->z = (double) _z;
    this->w = CalcW();
}

double idQuat::CalcW() const
{
    // take the absolute value because floating point rounding may cause the dot of x,y,z to be larger than 1
    return (double) sqrt(fabs(1.0f - (x * x + y * y + z * z)));
}

idStr idQuat::ToString() const
{
    return idMath::ToString(x) + " " + idMath::ToString(y) + " " + idMath::ToString(z);
}

void idAngles::ToVectors(idVec3 *forward, idVec3 *right, idVec3 *up) const
{
    double sr, sp, sy, cr, cp, cy;

    idMath::SinCos(DEG2RAD(yaw), sy, cy);
    idMath::SinCos(DEG2RAD(pitch), sp, cp);
    idMath::SinCos(DEG2RAD(roll), sr, cr);

    if (forward) {
        forward->Set(cp * cy, cp * sy, -sp);
    }

    if (right) {
        right->Set(-sr * sp * cy + cr * sy, -sr * sp * sy + -cr * cy, -sr * cp);
    }

    if (up) {
        up->Set(cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp);
    }
}

idVec3 idAngles::ToForward() const
{
    double sp, sy, cp, cy;

    idMath::SinCos(DEG2RAD(yaw), sy, cy);
    idMath::SinCos(DEG2RAD(pitch), sp, cp);

    return {cp * cy, cp * sy, -sp};
}

idQuat idAngles::ToQuat() const
{
	double sx, cx, sy, cy, sz, cz;
	double sxcy, cxcy, sxsy, cxsy;

	idMath::SinCos(DEG2RAD(yaw) * 0.5f, sz, cz);
	idMath::SinCos(DEG2RAD(pitch) * 0.5f, sy, cy);
	idMath::SinCos(DEG2RAD(roll) * 0.5f, sx, cx);

	sxcy = sx * cy;
	cxcy = cx * cy;
	sxsy = sx * sy;
	cxsy = cx * sy;

	return {cxsy*sz - sxcy*cz, -cxsy*cz - sxcy*sz, sxsy*cz - cxcy*sz, cxcy*cz + sxsy*sz};
}

idAngles idMat3::ToAngles() const
{
	idAngles	angles;
	double		theta;
	double		cp;
	double		sp;

	sp = mat[ 0 ][ 2 ];

	// cap off our sin value so that we don't get any NANs
	if (sp > 1.0f) {
		sp = 1.0f;
	} else if (sp < -1.0f) {
		sp = -1.0f;
	}

	theta = -asin(sp);
	cp = cos(theta);

	if (cp > 8192.0f * FLT_EPSILON) {
		angles.pitch	= RAD2DEG(theta);
		angles.yaw		= RAD2DEG(atan2(mat[ 0 ][ 1 ], mat[ 0 ][ 0 ]));
		angles.roll		= RAD2DEG(atan2(mat[ 1 ][ 2 ], mat[ 2 ][ 2 ]));
	} else {
		angles.pitch	= RAD2DEG(theta);
		angles.yaw		= RAD2DEG(-atan2(mat[ 1 ][ 0 ], mat[ 1 ][ 1 ]));
		angles.roll		= 0;
	}

	return angles;
}

idMat3 idQuat::ToMat3() const
{
	idMat3	mat;
	double	wx, wy, wz;
	double	xx, yy, yz;
	double	xy, xz, zz;
	double	x2, y2, z2;

	x2 = x + x;
	y2 = y + y;
	z2 = z + z;

	xx = x * x2;
	xy = x * y2;
	xz = x * z2;

	yy = y * y2;
	yz = y * z2;
	zz = z * z2;

	wx = w * x2;
	wy = w * y2;
	wz = w * z2;

	mat.mat[ 0 ][ 0 ] = 1.0f - (yy + zz);
	mat.mat[ 0 ][ 1 ] = xy - wz;
	mat.mat[ 0 ][ 2 ] = xz + wy;

	mat.mat[ 1 ][ 0 ] = xy + wz;
	mat.mat[ 1 ][ 1 ] = 1.0f - (xx + zz);
	mat.mat[ 1 ][ 2 ] = yz - wx;

	mat.mat[ 2 ][ 0 ] = xz - wy;
	mat.mat[ 2 ][ 1 ] = yz + wx;
	mat.mat[ 2 ][ 2 ] = 1.0f - (xx + yy);

	return mat;
}

idVec3 & idVec3::operator+=(const idVec3 &b)
{
    this->x += b.x;
    this->y += b.y;
    this->z += b.z;
    return *this;
}

idVec3 operator+(const idVec3 &a, const idVec3 &b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

idVec3 & idVec3::operator*=(double d)
{
    this->x *= d;
    this->y *= d;
    this->z *= d;
    return *this;
}

void idVec3::Rotate(const idVec3 &r)
{
    idQuat quat;
    quat.Set(x, y, z);
    idAngles angles = quat.ToAngles();

    idQuat quat2;
    quat2.Set(r.x, r.y, r.z);
    idAngles angles2 = quat.ToAngles();

    angles.pitch += angles2.pitch;
    angles.yaw += angles2.yaw;
    angles.roll += angles2.roll;
    quat = angles.ToQuat();

    //quat.ToForward()
    x = quat.x;
    y = quat.y;
    z = quat.z;
}

idVec3::idVec3(const char *str)
{
    idStr p = str;
    p.trimSelf();
    x = idLexer::GetDouble(p, &p);
    y = idLexer::GetDouble(p, &p);
    z = idLexer::GetDouble(p, &p);
}