#ifndef VECTOR_H
#define VECTOR_H

#include "Tuple.h"

namespace MiniBall
{
	class Point;

	class Vector : public Tuple<3>
	{
	public:
		Vector();
		Vector(const int i);
		Vector(const Vector &v);
		Vector(const Point &p);
		Vector(float v_x, float v_y, float v_z);

		Vector &operator=(const Vector &v);

		float getX() const;
		void setX(float x);
		__declspec(property(get = getX, put = setX)) float x;

		float getY() const;
		void setY(float y);
		__declspec(property(get = getY, put = setY)) float y;

		float getZ() const;
		void setZ(float z);
		__declspec(property(get = getZ, put = setZ)) float z;

		Vector operator+() const;
		Vector operator-() const;

		Vector &operator+=(const Vector &v);
		Vector &operator-=(const Vector &v);
		Vector &operator*=(float s);
		Vector &operator/=(float s);

		friend bool operator==(const Vector &u, const Vector &v);
		friend bool operator!=(const Vector &u, const Vector &v);

		friend Vector operator+(const Vector &u, const Vector &v);
		friend Vector operator-(const Vector &u, const Vector &v);
		friend float operator*(const Vector &u, const Vector &v);   // Dot product
		friend Vector operator*(float s, const Vector &v);
		friend Vector operator*(const Vector &v, float s);
		friend Vector operator/(const Vector &v, float s);
		friend float operator^(const Vector &v, const int n);   // Vector power
		friend float operator^(const Vector &u, const Vector &v);   // Angle between vectors
		friend Vector operator%(const Vector &u, const Vector &v);   // Cross product

		friend float N(const Vector &v);   // Norm
		friend float N2(const Vector &v);   // Squared norm
	};
}

#include "Point.h"

#include <assert.h>

namespace MiniBall
{
	inline Vector::Vector()
	{
	}

	inline Vector::Vector(const int i)
	{
		const float s = (float)i;

		x = s;
		y = s;
		z = s;
	}

	inline Vector::Vector(const Vector &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	inline Vector::Vector(const Point &P)
	{
		x = P.x;
		y = P.y;
		z = P.z;
	}

	inline Vector::Vector(float v_x, float v_y, float v_z)
	{
		x = v_x;
		y = v_y;
		z = v_z;
	}

	inline Vector &Vector::operator=(const Vector &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;

		return *this;
	}

	inline float Vector::getX() const
	{
		return element[0];
	}

	inline void Vector::setX(float x_)
	{
		element[0] = x_;
	}

	inline float Vector::getY() const
	{
		return element[1];
	}

	inline void Vector::setY(float y_)
	{
		element[1] = y_;
	}

	inline float Vector::getZ() const
	{
		return element[2];
	}

	inline void Vector::setZ(float z_)
	{
		element[2] = z_;
	}

	inline float operator^(const Vector &v, const int n)
	{
		switch(n)
		{
		case 2:
			return v*v;
		default:
			assert(n == 2);   // FIXME: Compile time assert?
			return 1;
		}
	}
}

#endif   // VECTOR_H
