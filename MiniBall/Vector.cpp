#include "stdafx.h"

#include "Vector.h"

#include <math.h>

namespace MiniBall
{
	Vector Vector::operator+() const
	{
		return *this;
	}

	Vector Vector::operator-() const
	{
		return Vector(-x, -y, -z);
	}

	Vector &Vector::operator+=(const Vector &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}

	Vector &Vector::operator-=(const Vector &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

	Vector &Vector::operator*=(float s)
	{
		x *= s;
		y *= s;
		z *= s;

		return *this;
	}

	Vector &Vector::operator/=(float s)
	{
		float r = 1.0f / s;

		return *this *= r;
	}

	bool operator==(const Vector &U, const Vector &v)
	{
		if(U.x == v.x && U.y == v.y && U.z == v.z)
			return true;
		else
			return false;
	}

	bool operator!=(const Vector &U, const Vector &v)
	{
		if(U.x != v.x || U.y != v.y || U.z != v.z)
			return true;
		else
			return false;
	}

	Vector operator+(const Vector &u, const Vector &v)
	{
		return Vector(u.x + v.x, u.y + v.y, u.z + v.z);
	}

	Vector operator-(const Vector &u, const Vector &v)
	{
		return Vector(u.x - v.x, u.y - v.y, u.z - v.z);
	}

	float operator*(const Vector &u, const Vector &v)
	{
		return u.x * v.x + u.y * v.y + u.z * v.z;
	}

	Vector operator*(float s, const Vector &v)
	{
		return Vector(s * v.x, s * v.y, s * v.z);
	}

	Vector operator*(const Vector &v, float s)
	{
		return Vector(v.x * s, v.y * s, v.z * s);
	}

	Vector operator/(const Vector &v, float s)
	{
		float r = 1.0f / s;

		return Vector(v.x * r, v.y * r, v.z * r);
	}

	float operator^(const Vector &u, const Vector &v)
	{
		return acosf(u / N(u) * v / N(v));
	}

	Vector operator%(const Vector &u, const Vector &v)
	{
		return Vector(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
	}

	float N(const Vector &v)
	{
		return sqrtf(v^2);
	}

	float N2(const Vector &v)
	{
		return v^2;
	}
}