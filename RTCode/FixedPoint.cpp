/*======================================================

FixedPoint.cpp

a fixed point math class

======================================================*/

#include "FixedPoint.h"

inline Fixed operator +(float a, const Fixed& b) { return Fixed(a)+b; }
inline Fixed operator -(float a, const Fixed& b) { return Fixed(a)-b; }
inline Fixed operator *(float a, const Fixed& b) { return Fixed(a)*b; }
inline Fixed operator /(float a, const Fixed& b) { return Fixed(a)/b; }

inline bool operator ==(float a, const Fixed& b) { return Fixed(a) == b; }
inline bool operator !=(float a, const Fixed& b) { return Fixed(a) != b; }
inline bool operator <=(float a, const Fixed& b) { return Fixed(a) <= b; }
inline bool operator >=(float a, const Fixed& b) { return Fixed(a) >= b; }
inline bool operator  <(float a, const Fixed& b) { return Fixed(a)  < b; }
inline bool operator  >(float a, const Fixed& b) { return Fixed(a)  > b; }



inline Fixed operator +(double a, const Fixed& b) { return Fixed(a)+b; }
inline Fixed operator -(double a, const Fixed& b) { return Fixed(a)-b; }
inline Fixed operator *(double a, const Fixed& b) { return Fixed(a)*b; }
inline Fixed operator /(double a, const Fixed& b) { return Fixed(a)/b; }

inline bool operator ==(double a, const Fixed& b) { return Fixed(a) == b; }
inline bool operator !=(double a, const Fixed& b) { return Fixed(a) != b; }
inline bool operator <=(double a, const Fixed& b) { return Fixed(a) <= b; }
inline bool operator >=(double a, const Fixed& b) { return Fixed(a) >= b; }
inline bool operator  <(double a, const Fixed& b) { return Fixed(a)  < b; }
inline bool operator  >(double a, const Fixed& b) { return Fixed(a)  > b; }


int& operator +=(int& a, const Fixed& b) { a = (Fixed)a + b; return a; }
int& operator -=(int& a, const Fixed& b) { a = (Fixed)a - b; return a; }
int& operator *=(int& a, const Fixed& b) { a = (Fixed)a * b; return a; }
int& operator /=(int& a, const Fixed& b) { a = (Fixed)a / b; return a; }

long& operator +=(long& a, const Fixed& b) { a = (Fixed)a + b; return a; }
long& operator -=(long& a, const Fixed& b) { a = (Fixed)a - b; return a; }
long& operator *=(long& a, const Fixed& b) { a = (Fixed)a * b; return a; }
long& operator /=(long& a, const Fixed& b) { a = (Fixed)a / b; return a; }

float& operator +=(float& a, const Fixed& b) { a = a + b; return a; }
float& operator -=(float& a, const Fixed& b) { a = a - b; return a; }
float& operator *=(float& a, const Fixed& b) { a = a * b; return a; }
float& operator /=(float& a, const Fixed& b) { a = a / b; return a; }

double& operator +=(double& a, const Fixed& b) { a = a + b; return a; }
double& operator -=(double& a, const Fixed& b) { a = a - b; return a; }
double& operator *=(double& a, const Fixed& b) { a = a * b; return a; }
double& operator /=(double& a, const Fixed& b) { a = a / b; return a; }

Fixed Fixed::MAXVALUE()
{
	return Fixed(RAW,MaxRawValue);
}

/*

CFixedPoint::CFixedPoint(float fValue)
{
	m_nValue = (unsigned int)(fValue * 65536.0f);
}

CFixedPoint::CFixedPoint()
{
	m_nValue = 0;
}

CFixedPoint::operator float()
{
	return ((float)m_nValue) / 65536.0f;
}

CFixedPoint::operator int()
{
	return m_nValue / 65536;
}

bool CFixedPoint::operator < (const CFixedPoint &other)
{
	return m_nValue < other.m_nValue;
}

bool CFixedPoint::operator > (const CFixedPoint &other)
{
	return m_nValue > other.m_nValue;
}

CFixedPoint CFixedPoint::operator / (const CFixedPoint &other)
{
	return ((m_nValue << 6) / (other.m_nValue >> 6)) << 4;
}

CFixedPoint CFixedPoint::operator * (const CFixedPoint &other)
{
	return m_nValue * other.m_nValue;
}

CFixedPoint CFixedPoint::ceilf()
{
	CFixedPoint ret;

	ret.m_nValue = m_nValue & 0xFFFF0000;

	if(m_nValue & 0xFFFF)
	{
		ret.m_nValue += 0x00010000;
	}

	return ret;
}

CFixedPoint CFixedPoint::floorf()
{
	CFixedPoint ret;

	ret.m_nValue = m_nValue & 0xFFFF0000;

	if((m_nValue & 0xFFFF) == 0)
	{
		ret.m_nValue -= 0x00010000;
	}

	return ret;
}
*/