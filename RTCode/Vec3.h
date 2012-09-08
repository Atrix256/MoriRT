/*======================================================

Vec3.h

3 vector for math purposes

======================================================*/

#ifndef VEC3_H
#define VEC3_H

#include "Defines.h"

class Vec3
{
public:
	Vec3()
	:m_fX(0.0f)
	,m_fY(0.0f)
	,m_fZ(0.0f)
	{
	}

	Vec3(MRTFLOAT x, MRTFLOAT y, MRTFLOAT z)
	{
		m_fX = x;
		m_fY = y;
		m_fZ = z;
	}

	void SetColorFromBGRA(unsigned int nColor);

	Vec3 operator - (Vec3 vOther) const;
	Vec3 operator + (Vec3 vOther) const;
	Vec3 operator * (Vec3 vOther) const;
	Vec3 operator / (Vec3 vOther) const;
	
	Vec3 operator -= (Vec3 vOther);
	Vec3 operator += (Vec3 vOther);
	Vec3 operator *= (Vec3 vOther);
	Vec3 operator /= (Vec3 vOther);

	Vec3 operator * (MRTFLOAT fScalar) const;
	Vec3 operator / (MRTFLOAT fScalar) const;

	Vec3 operator *= (MRTFLOAT fScalar);
	Vec3 operator /= (MRTFLOAT fScalar);

	MRTFLOAT &operator[] (int nIndex);
	const MRTFLOAT &operator[] (int nIndex) const;

	Vec3 Cross(const Vec3 &vOther) const;
	MRTFLOAT Dot(const Vec3 &vOther) const;

	MRTFLOAT GetMonochromeFromRGB() const;

	Vec3 &Normalize();
	MRTFLOAT Length() const;
	MRTFLOAT LengthSq() const;

	MRTFLOAT m_fX;
	MRTFLOAT m_fY;
	MRTFLOAT m_fZ;
};

#endif //VEC3_H
