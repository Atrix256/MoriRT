/*======================================================

Vec3.cpp

3 vector for math purposes

======================================================*/

#include "Vec3.h"
#include <math.h>

Vec3 Vec3::operator - (Vec3 vOther) const
{
	Vec3 vRet(m_fX - vOther.m_fX, m_fY - vOther.m_fY, m_fZ - vOther.m_fZ);
	return vRet;
}

Vec3 Vec3::operator + (Vec3 vOther) const
{
	Vec3 vRet(m_fX + vOther.m_fX, m_fY + vOther.m_fY, m_fZ + vOther.m_fZ);
	return vRet;
}

Vec3 Vec3::operator * (Vec3 vOther) const
{
	Vec3 vRet(m_fX * vOther.m_fX, m_fY * vOther.m_fY, m_fZ * vOther.m_fZ);
	return vRet;
}

Vec3 Vec3::operator / (Vec3 vOther) const
{
	Vec3 vRet(m_fX / vOther.m_fX, m_fY / vOther.m_fY, m_fZ / vOther.m_fZ);
	return vRet;
}

Vec3 Vec3::operator -= (Vec3 vOther)
{
	m_fX -= vOther.m_fX;
	m_fY -= vOther.m_fY;
	m_fZ -= vOther.m_fZ;

	return *this;
}

Vec3 Vec3::operator += (Vec3 vOther)
{
	m_fX += vOther.m_fX;
	m_fY += vOther.m_fY;
	m_fZ += vOther.m_fZ;

	return *this;
}

Vec3 Vec3::operator *= (Vec3 vOther)
{
	m_fX *= vOther.m_fX;
	m_fY *= vOther.m_fY;
	m_fZ *= vOther.m_fZ;

	return *this;
}

Vec3 Vec3::operator /= (Vec3 vOther)
{
	m_fX /= vOther.m_fX;
	m_fY /= vOther.m_fY;
	m_fZ /= vOther.m_fZ;

	return *this;
}

Vec3 Vec3::operator * (MRTFLOAT fScalar) const
{
	Vec3 vRet(m_fX * fScalar, m_fY * fScalar, m_fZ * fScalar);
	return vRet;
}

Vec3 Vec3::operator / (MRTFLOAT fScalar) const
{
	Vec3 vRet(m_fX / fScalar, m_fY / fScalar, m_fZ / fScalar);
	return vRet;
}

Vec3 Vec3::operator *= (MRTFLOAT fScalar)
{
	m_fX *= fScalar;
	m_fY *= fScalar;
	m_fZ *= fScalar;
	return *this;
}

Vec3 Vec3::operator /= (MRTFLOAT fScalar)
{
	m_fX /= fScalar;
	m_fY /= fScalar;
	m_fZ /= fScalar;
	return *this;
}

Vec3 &Vec3::Normalize()
{
	MRTFLOAT fLength = Length();
	m_fX /= fLength;
	m_fY /= fLength;
	m_fZ /= fLength;

	return *this;
}

MRTFLOAT Vec3::Length() const
{
	return sqrtf((m_fX*m_fX)+(m_fY*m_fY)+(m_fZ*m_fZ));
}

MRTFLOAT Vec3::LengthSq() const
{
	return (m_fX*m_fX)+(m_fY*m_fY)+(m_fZ*m_fZ);
}

Vec3 Vec3::Cross(const Vec3 &vOther) const
{
	Vec3 vRet;
	
	vRet.m_fX = m_fY * vOther.m_fZ - m_fZ * vOther.m_fY;
	vRet.m_fY = m_fZ * vOther.m_fX - m_fX * vOther.m_fZ;
	vRet.m_fZ = m_fX * vOther.m_fY - m_fY * vOther.m_fX;

	return vRet;
}

MRTFLOAT Vec3::Dot(const Vec3 &vOther) const
{
	return m_fX * vOther.m_fX + m_fY * vOther.m_fY + m_fZ * vOther.m_fZ;
}

MRTFLOAT &Vec3::operator[] (int nIndex)
{
	switch(nIndex)
	{
		case 0:
		{
			return m_fX;
		}
		case 1:
		{
			return m_fY;
		}
		case 2:
		default:
		{
			return m_fZ;
		}
	}
}

const MRTFLOAT &Vec3::operator[] (int nIndex) const
{
	switch(nIndex)
	{
		case 0:
		{
			return m_fX;
		}
		case 1:
		{
			return m_fY;
		}
		case 2:
		default:
		{
			return m_fZ;
		}
	}
}

void Vec3::SetColorFromBGRA(unsigned int nColor)
{
	m_fX = ((MRTFLOAT)(float)((nColor & 0xFF0000) >> 16)) / 255.0f;
	m_fY = ((MRTFLOAT)(float)((nColor & 0xFF00) >> 8)) / 255.0f;
	m_fZ = ((MRTFLOAT)(float)((nColor & 0xFF))) / 255.0f;
}

MRTFLOAT Vec3::GetMonochromeFromRGB() const
{
	//convert from RGB to greyscale: http://en.wikipedia.org/wiki/Grayscale#Converting_color_to_grayscale
	return (m_fX * 0.30f + m_fY * 0.59f + m_fZ * 0.11f);
}