#include "stdafx.h"

#include "Point.h"

namespace MiniBall
{
	Point &Point::operator+=(const Vector &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}

	Point &Point::operator-=(const Vector &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

	Point operator+(const Point &P, const Vector &v)
	{
		return Point(P.x + v.x, P.y + v.y, P.z + v.z);
	}

	Point operator-(const Point &P, const Vector &v)
	{
		return Point(P.x - v.x, P.y - v.y, P.z - v.z);
	}

	Vector operator-(const Point &P, const Point &Q)
	{
		return Vector(P.x - Q.x, P.y - Q.y, P.z - Q.z);
	}

	float Point::d(const Point &P) const
	{
		return N(*this - P);
	}

	float Point::d2(const Point &P) const
	{
		return N2(*this - P);
	}

	float Point::d(const Point &P, const Point &Q)
	{
		return N(P - Q);
	}

	float Point::d2(const Point &P, const Point &Q)
	{
		return N2(P - Q);
	}
}