#include "stdafx.h"

#include "Quaternion.h"

namespace MiniBall
{
	Quaternion::operator float() const
	{
		return w;
	}

	Quaternion::operator Vector() const
	{
		return Vector(x, y, z);
	}

	Quaternion::operator Matrix() const
	{
		return Matrix(1 - 2 * (y * y + z * z), 2 * (x * y - w * z),     2 * (x * z + w * y),
		              2 * (x * y + w * z),     1 - 2 * (x * x + z * z), 2 * (y * z - w * x),
		              2 * (x * z - w * y),     2 * (y * z + w * x),     1 - 2 * (x * x + y * y));
	}

	Quaternion Quaternion::operator+() const
	{
		return *this;
	}

	Quaternion Quaternion::operator-() const
	{
		return Quaternion(-x, -y, -z, -w);
	}

	Quaternion Quaternion::operator!() const
	{
		return ~(*this) / N2(*this);
	}

	Quaternion Quaternion::operator~() const
	{
		return Quaternion(-x, -y, -z, w);
	}

	Quaternion &Quaternion::operator+=(const Quaternion &Q)
	{
		x += Q.x;
		y += Q.y;
		z += Q.z;
		w += Q.w;

		return *this;
	}

	Quaternion &Quaternion::operator-=(const Quaternion &Q)
	{
		x -= Q.x;
		y -= Q.y;
		z -= Q.z;
		w -= Q.w;

		return *this;
	}

	Quaternion &Quaternion::operator*=(float s)
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;

		return *this;
	}

	Quaternion &Quaternion::operator*=(const Quaternion &Q)
	{
		float Qx = w * Q.x + x * Q.w + y * Q.z - z * Q.y;
		float Qy = w * Q.y - x * Q.z + y * Q.w + z * Q.x;
		float Qz = w * Q.z + x * Q.y - y * Q.x + z * Q.w;
		float Qw = w * Q.w - x * Q.x - y * Q.y - z * Q.z;

		w = Qw;
		x = Qx;
		y = Qy;
		z = Qz;

		return (*this);
	}

	Quaternion &Quaternion::operator/=(float s)
	{
		float r = 1.0f / s;

		return *this *= r;
	}

	bool operator==(const Quaternion &Q, const Quaternion &R)
	{
		if(Q.x == R.x && Q.y == R.y && Q.z == R.z && Q.w == R.w)
			return true;
		else
			return false;
	}

	bool operator!=(const Quaternion &Q, const Quaternion &R)
	{
		if(Q.x != R.x || Q.y != R.y || Q.z != R.z || Q.w != R.w)
			return true;
		else
			return false;
	}

	Quaternion operator+(const Quaternion &Q, const Quaternion &R)
	{
		return Quaternion(Q.x + R.x, Q.y + R.y, Q.z + R.z, Q.w + R.w);
	}

	Quaternion operator-(const Quaternion &Q, const Quaternion &R)
	{
		return Quaternion(Q.x - R.x, Q.y - R.y, Q.z - R.z, Q.w - R.w);
	}

	Quaternion operator*(const Quaternion &Q, const Quaternion &R)
	{
		float x = +Q.x * R.w + Q.y * R.z - Q.z * R.y + Q.w * R.x;
		float y = -Q.x * R.z + Q.y * R.w + Q.z * R.x + Q.w * R.y;
		float z = +Q.x * R.y - Q.y * R.x + Q.z * R.w + Q.w * R.z;
		float w = -Q.x * R.x - Q.y * R.y - Q.z * R.z + Q.w * R.w;

		return Quaternion(x, y, z, w);
	}

	Quaternion operator*(float s, const Quaternion &Q)
	{
		return Quaternion(s * Q.x, s * Q.y, s * Q.z, s * Q.w);
	}

	Quaternion operator*(const Quaternion &Q, float s)
	{
		return Quaternion(Q.x * s, Q.y * s, Q.z * s, Q.w * s);
	}

	Quaternion operator/(const Quaternion &Q, float s)
	{
		float r = 1.0f / s;

		return Q * r;
	}

	float N(const Quaternion &Q)
	{
		return sqrtf(Q.x*Q.x + Q.y*Q.y + Q.z*Q.z + Q.w*Q.w);
	}

	float N2(const Quaternion &Q)
	{
		return Q.x*Q.x + Q.y*Q.y + Q.z*Q.z + Q.w*Q.w;
	}

	Quaternion slerp(const Quaternion &Q, const Quaternion &R, float t)
	{
		return Quaternion(Q, R, t);
	}
}