// p_vector.h - yet another vector class.
//
// Copyright 1997 by Jonathan P. Leech
// Modifications Copyright 1997-1999 by David K. McAllister
//
// A simple 3D float vector class for internal use by the particle systems.

#ifndef particle_vector_h
#define particle_vector_h

#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433f
#endif

#define drand48() (((float) rand())/((float) RAND_MAX))
#define srand48(x) srand(x)

class pVector
{
public:
	float x, y, z;

	IC Fvector& xr(){return *(Fvector*)this;}
	IC const Fvector& xr() const {return *(Fvector*)this;}
	
	IC pVector(float ax, float ay, float az) : x(ax), y(ay), z(az){}
	
	IC pVector() {}
	
	IC float length() const
	{
		return _sqrt(x*x+y*y+z*z);
	}
	
	IC float length2() const
	{
		return (x*x+y*y+z*z);
	}
	
	IC float normalize()
	{
		float onel = 1.0f / _sqrt(x*x+y*y+z*z);
		x *= onel;
		y *= onel;
		z *= onel;
		
		return onel;
	}
	
	IC float operator*(const pVector &a) const
	{
		return x*a.x + y*a.y + z*a.z;
	}
	
	IC pVector operator*(const float s) const
	{
		return pVector(x*s, y*s, z*s);
	}
	
	IC pVector operator/(const float s) const
	{
		float invs = 1.0f / s;
		return pVector(x*invs, y*invs, z*invs);
	}
	
	IC pVector operator+(const pVector& a) const
	{
		return pVector(x+a.x, y+a.y, z+a.z);
	}
	
	IC pVector operator-(const pVector& a) const
	{
		return pVector(x-a.x, y-a.y, z-a.z);
	}
	
	IC pVector operator-()
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}
	
	IC pVector& operator+=(const pVector& a)
	{
		x += a.x;
		y += a.y;
		z += a.z;
		return *this;
	}
	
	IC pVector& operator-=(const pVector& a)
	{
		x -= a.x;
		y -= a.y;
		z -= a.z;
		return *this;
	}
	
	IC pVector& operator*=(const float a)
	{
		x *= a;
		y *= a;
		z *= a;
		return *this;
	}
	
	IC pVector& operator/=(const float a)
	{
		float b = 1.0f / a;
		x *= b;
		y *= b;
		z *= b;
		return *this;
	}
	
	IC pVector& operator=(const pVector& a)
	{
		x = a.x;
		y = a.y;
		z = a.z;
		return *this;
	}
	
	IC pVector operator^(const pVector& b) const
	{
		return pVector(
			y*b.z-z*b.y,
			z*b.x-x*b.z,
			x*b.y-y*b.x);
	}
};

#endif
