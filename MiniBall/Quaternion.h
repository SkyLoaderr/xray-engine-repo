#ifndef QUATERNION_H
#define QUATERNION_H

#include "Tuple.h"

namespace MiniBall
{
	class Point;
	class Vector;
	class Matrix;

	class Quaternion : public Tuple<4>
	{
	public:
		Quaternion();
		Quaternion(float Q_w);
		Quaternion(const Quaternion &Q);
		Quaternion(const Quaternion &Q, const Quaternion &R, float t);   // Slerp
		Quaternion(const Vector &Q_v, float Q_w = 0.0f);
		Quaternion(const Point &P, float Q_w = 1.0f);
		Quaternion(const Matrix &A);   // Must be a rotation Matrix
		Quaternion(float Q_x, float Q_y, float Q_z, float Q_w);

		Quaternion &operator=(const Quaternion &Q);

		float getX() const;
		void setX(float x);
		__declspec(property(get = getX, put = setX)) float x;

		float getY() const;
		void setY(float y);
		__declspec(property(get = getY, put = setY)) float y;

		float getZ() const;
		void setZ(float z);
		__declspec(property(get = getZ, put = setZ)) float z;

		float getW() const;
		void setW(float w);
		__declspec(property(get = getW, put = setW)) float w;

		operator float() const;
		operator Vector() const;
		operator Matrix() const;

		Quaternion operator+() const;
		Quaternion operator-() const;

		Quaternion operator!() const;   // Inverse
		Quaternion operator~() const;   // Conjugate

		Quaternion &operator+=(const Quaternion &V);
		Quaternion &operator-=(const Quaternion &V);
		Quaternion &operator*=(float s);
		Quaternion &operator*=(const Quaternion &Q);
		Quaternion &operator/=(float s);

		friend bool operator==(const Quaternion &Q, const Quaternion &R);
		friend bool operator!=(const Quaternion &Q, const Quaternion &R);

		friend Quaternion operator+(const Quaternion &Q, const Quaternion &R);
		friend Quaternion operator-(const Quaternion &Q, const Quaternion &R);
		friend Quaternion operator*(const Quaternion &Q, const Quaternion &R);
		friend Quaternion operator*(float s, const Quaternion &Q);
		friend Quaternion operator*(const Quaternion &Q, float s);
		friend Quaternion operator/(const Quaternion &Q, float s);

		friend float N(const Quaternion &Q);   // Norm
		friend float N2(const Quaternion &Q);   // Squared norm
		friend Quaternion slerp(const Quaternion &Q, const Quaternion &R, float t);
	};
}

#include "Matrix.h"
#include "Point.h"
#include "Vector.h"

#include <math.h>

namespace MiniBall
{
	inline Quaternion::Quaternion()
	{
	}

	inline Quaternion::Quaternion(const Quaternion &Q)
	{
		x = Q.x;
		y = Q.y;
		z = Q.z;
		w = Q.w;
	}

	inline Quaternion::Quaternion(const Quaternion &Q, const Quaternion &R, float t)
	{
		float a = acosf(Q.x * R.x + Q.y * R.y + Q.z * R.z + Q.w * R.w);   // Angle
		float s = sinf(a);

		*this = (Q * sinf(a * (1 - t)) + R * sinf(a * t)) / s;
	}

	inline Quaternion::Quaternion(float Q_w)
	{
		x = 0;
		y = 0;
		z = 0;
		w = Q_w;
	}

	inline Quaternion::Quaternion(const Vector &Q_v, float Q_w)
	{
		x = Q_v.x;
		y = Q_v.y;
		z = Q_v.z;
		w = Q_w;
	}

	inline Quaternion::Quaternion(const Point &P, float Q_w)
	{
		x = P.x;
		y = P.y;
		z = P.z;
		w = Q_w;
	}

	inline Quaternion::Quaternion(const Matrix &A)
	{
		w = 0.5F * sqrtf(Matrix::tr(A));
		x = (A(3, 2) - A(2, 3)) / (4 * w);
		y = (A(1, 3) - A(3, 1)) / (4 * w);
		z = (A(2, 1) - A(1, 2)) / (4 * w);
	}

	inline Quaternion::Quaternion(float Q_x, float Q_y, float Q_z, float Q_w)
	{
		x = Q_x;
		y = Q_y;
		z = Q_z;
		w = Q_w;
	}

	inline Quaternion &Quaternion::operator=(const Quaternion &Q)
	{
		x = Q.x;
		y = Q.y;
		z = Q.z;
		w = Q.w;

		return *this;
	}

	inline float Quaternion::getX() const
	{
		return element[0];
	}

	inline void Quaternion::setX(float x_)
	{
		element[0] = x_;
	}

	inline float Quaternion::getY() const
	{
		return element[1];
	}

	inline void Quaternion::setY(float y_)
	{
		element[1] = y_;
	}

	inline float Quaternion::getZ() const
	{
		return element[2];
	}

	inline void Quaternion::setZ(float z_)
	{
		element[2] = z_;
	}

	inline float Quaternion::getW() const
	{
		return element[3];
	}

	inline void Quaternion::setW(float w_)
	{
		element[3] = w_;
	}
}

#endif   // QUATERNION_H