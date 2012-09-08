/*======================================================

FixedPoint.h

a fixed point math class

======================================================*/

#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H

/*

class CFixedPoint
{
public:
	CFixedPoint();
	CFixedPoint(float fValue);

	operator float();
	operator int();

	bool operator < (const CFixedPoint &other);
	bool operator < (float fValue) {return ((float)*this) < fValue;}
	bool operator < (int nValue) {return ((int)*this) < nValue;}

	bool operator > (const CFixedPoint &other);
	bool operator > (float fValue) {return ((float)*this) > fValue;}
	bool operator > (int nValue) {return ((int)*this) > nValue;}

	CFixedPoint operator / (const CFixedPoint &other);
	CFixedPoint operator * (const CFixedPoint &other);

	CFixedPoint ceilf();
	CFixedPoint floorf();

private:
	unsigned int m_nValue;
};

CFixedPoint ceilf(CFixedPoint value) {return value.ceilf();}
CFixedPoint floorf(CFixedPoint value) {return value.floorf();}
*/



/*
Copyright (c) 2006 Henry Strickland & Ryan Seto

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

        (* http://www.opensource.org/licenses/mit-license.php *)
*/

class Fixed {

private:

	int	g; // the guts

const static int BP= 16;  // how many low bits are right of Binary Point
const static int BP2= BP*2;  // how many low bits are right of Binary Point
const static int BPhalf= BP/2;  // how many low bits are right of Binary Point
const static int MaxRawValue = 0x7FFFFFFF;

static double STEP() { return 1.0 / (1<<BP); }  // smallest step we can represent

	// for private construction via guts
	enum FixedRaw { RAW };
	Fixed(FixedRaw, int guts) : g(guts) {}


public:
	Fixed() : g(0) {}
	Fixed(const Fixed& a) : g( a.g ) {}
	Fixed(float a) : g( int(a / (float)STEP()) ) {}
	Fixed(double a) : g( int(a / (double)STEP()) ) {}
	Fixed(int a) : g( a << BP ) {}
	Fixed(long a) : g( a << BP ) {}
	Fixed& operator =(const Fixed& a) { g= a.g; return *this; }
	Fixed& operator =(float a) { g= Fixed(a).g; return *this; }
	Fixed& operator =(double a) { g= Fixed(a).g; return *this; }
	Fixed& operator =(int a) { g= Fixed(a).g; return *this; }
	Fixed& operator =(long a) { g= Fixed(a).g; return *this; }

	static Fixed MAXVALUE();

	operator float() { return g * (float)STEP(); }
	operator double() { return g * (double)STEP(); }
	operator int() { return g>>BP; }
	operator long() { return g>>BP; }
	operator unsigned char() {return g>>BP;}

	operator float() const { return g * (float)STEP(); }

	Fixed operator +() const { return Fixed(RAW,g); }
	Fixed operator -() const { return Fixed(RAW,-g); }

	Fixed operator +(const Fixed& a) const { return Fixed(RAW, g + a.g); }
	Fixed operator -(const Fixed& a) const { return Fixed(RAW, g - a.g); }
#if 1
	// more acurate, using long long
	Fixed operator *(const Fixed& a) const { return Fixed(RAW,  (int)( ((long long)g * (long long)a.g ) >> BP)); }
#else
	// faster, but with only half as many bits right of binary point
	Fixed operator *(const Fixed& a) const { return Fixed(RAW, (g>>BPhalf) * (a.g>>BPhalf) ); }
#endif
	Fixed operator /(const Fixed& a) const { return Fixed(RAW, int( (((long long)g << BP2) / (long long)(a.g)) >> BP) ); }

	Fixed operator +(float a) const { return Fixed(RAW, g + Fixed(a).g); }
	Fixed operator -(float a) const { return Fixed(RAW, g - Fixed(a).g); }
	Fixed operator *(float a) const { return Fixed(RAW, (g>>BPhalf) * (Fixed(a).g>>BPhalf) ); }
	Fixed operator /(float a) const { return Fixed(RAW, int( (((long long)g << BP2) / (long long)(Fixed(a).g)) >> BP) ); }

	Fixed operator +(double a) const { return Fixed(RAW, g + Fixed(a).g); }
	Fixed operator -(double a) const { return Fixed(RAW, g - Fixed(a).g); }
	Fixed operator *(double a) const { return Fixed(RAW, (g>>BPhalf) * (Fixed(a).g>>BPhalf) ); }
	Fixed operator /(double a) const { return Fixed(RAW, int( (((long long)g << BP2) / (long long)(Fixed(a).g)) >> BP) ); }

	Fixed& operator +=(Fixed a) { *this = *this + a; return *this; }
	Fixed& operator -=(Fixed a) { *this = *this - a; return *this; }
	Fixed& operator *=(Fixed a) { *this = *this * a; return *this; }
	Fixed& operator /=(Fixed a) { *this = *this / a; return *this; }

	Fixed& operator +=(int a) { *this = *this + (Fixed)a; return *this; }
	Fixed& operator -=(int a) { *this = *this - (Fixed)a; return *this; }
	Fixed& operator *=(int a) { *this = *this * (Fixed)a; return *this; }
	Fixed& operator /=(int a) { *this = *this / (Fixed)a; return *this; }

	Fixed& operator +=(long a) { *this = *this + (Fixed)a; return *this; }
	Fixed& operator -=(long a) { *this = *this - (Fixed)a; return *this; }
	Fixed& operator *=(long a) { *this = *this * (Fixed)a; return *this; }
	Fixed& operator /=(long a) { *this = *this / (Fixed)a; return *this; }

	Fixed& operator +=(float a) { *this = *this + a; return *this; }
	Fixed& operator -=(float a) { *this = *this - a; return *this; }
	Fixed& operator *=(float a) { *this = *this * a; return *this; }
	Fixed& operator /=(float a) { *this = *this / a; return *this; }

	Fixed& operator +=(double a) { *this = *this + a; return *this; }
	Fixed& operator -=(double a) { *this = *this - a; return *this; }
	Fixed& operator *=(double a) { *this = *this * a; return *this; }
	Fixed& operator /=(double a) { *this = *this / a; return *this; }

	bool operator ==(const Fixed& a) const { return g == a.g; }
	bool operator !=(const Fixed& a) const { return g != a.g; }
	bool operator <=(const Fixed& a) const { return g <= a.g; }
	bool operator >=(const Fixed& a) const { return g >= a.g; }
	bool operator  <(const Fixed& a) const { return g  < a.g; }
	bool operator  >(const Fixed& a) const { return g  > a.g; }

	bool operator ==(float a) const { return g == Fixed(a).g; }
	bool operator !=(float a) const { return g != Fixed(a).g; }
	bool operator <=(float a) const { return g <= Fixed(a).g; }
	bool operator >=(float a) const { return g >= Fixed(a).g; }
	bool operator  <(float a) const { return g  < Fixed(a).g; }
	bool operator  >(float a) const { return g  > Fixed(a).g; }

	bool operator ==(double a) const { return g == Fixed(a).g; }
	bool operator !=(double a) const { return g != Fixed(a).g; }
	bool operator <=(double a) const { return g <= Fixed(a).g; }
	bool operator >=(double a) const { return g >= Fixed(a).g; }
	bool operator  <(double a) const { return g  < Fixed(a).g; }
	bool operator  >(double a) const { return g  > Fixed(a).g; }
};

inline Fixed operator +(float a, const Fixed& b);// { return Fixed(a)+b; }
inline Fixed operator -(float a, const Fixed& b);// { return Fixed(a)-b; }
inline Fixed operator *(float a, const Fixed& b);// { return Fixed(a)*b; }
inline Fixed operator /(float a, const Fixed& b);// { return Fixed(a)/b; }

inline bool operator ==(float a, const Fixed& b);// { return Fixed(a) == b; }
inline bool operator !=(float a, const Fixed& b);// { return Fixed(a) != b; }
inline bool operator <=(float a, const Fixed& b);// { return Fixed(a) <= b; }
inline bool operator >=(float a, const Fixed& b);// { return Fixed(a) >= b; }
inline bool operator  <(float a, const Fixed& b);// { return Fixed(a)  < b; }
inline bool operator  >(float a, const Fixed& b);// { return Fixed(a)  > b; }



inline Fixed operator +(double a, const Fixed& b);// { return Fixed(a)+b; }
inline Fixed operator -(double a, const Fixed& b);// { return Fixed(a)-b; }
inline Fixed operator *(double a, const Fixed& b);// { return Fixed(a)*b; }
inline Fixed operator /(double a, const Fixed& b);// { return Fixed(a)/b; }

inline bool operator ==(double a, const Fixed& b);// { return Fixed(a) == b; }
inline bool operator !=(double a, const Fixed& b);// { return Fixed(a) != b; }
inline bool operator <=(double a, const Fixed& b);// { return Fixed(a) <= b; }
inline bool operator >=(double a, const Fixed& b);// { return Fixed(a) >= b; }
inline bool operator  <(double a, const Fixed& b);// { return Fixed(a)  < b; }
inline bool operator  >(double a, const Fixed& b);// { return Fixed(a)  > b; }


int& operator +=(int& a, const Fixed& b);// { a = (Fixed)a + b; return a; }
int& operator -=(int& a, const Fixed& b);// { a = (Fixed)a - b; return a; }
int& operator *=(int& a, const Fixed& b);// { a = (Fixed)a * b; return a; }
int& operator /=(int& a, const Fixed& b);// { a = (Fixed)a / b; return a; }

long& operator +=(long& a, const Fixed& b);// { a = (Fixed)a + b; return a; }
long& operator -=(long& a, const Fixed& b);// { a = (Fixed)a - b; return a; }
long& operator *=(long& a, const Fixed& b);// { a = (Fixed)a * b; return a; }
long& operator /=(long& a, const Fixed& b);// { a = (Fixed)a / b; return a; }

float& operator +=(float& a, const Fixed& b);// { a = a + b; return a; }
float& operator -=(float& a, const Fixed& b);// { a = a - b; return a; }
float& operator *=(float& a, const Fixed& b);// { a = a * b; return a; }
float& operator /=(float& a, const Fixed& b);// { a = a / b; return a; }

double& operator +=(double& a, const Fixed& b);// { a = a + b; return a; }
double& operator -=(double& a, const Fixed& b);// { a = a - b; return a; }
double& operator *=(double& a, const Fixed& b);// { a = a * b; return a; }
double& operator /=(double& a, const Fixed& b);// { a = a / b; return a; }

#endif //FIXEDPOINT_H