#ifndef POINT_H
#define POINT_H

#include "Tuple.h"

namespace MiniBall
{
	class Vector;

	class Point : public Tuple<3>
	{
	public:
		Point();
		Point(const int i);
		Point(const Point &P);
		Point(const Vector &v);
		Point(float Px, float Py, float Pz);

		Point &operator=(const Point &P);

		float getX() const;
		void setX(float x);
		__declspec(property(get = getX, put = setX)) float x;

		float getY() const;
		void setY(float y);
		__declspec(property(get = getY, put = setY)) float y;

		float getZ() const;
		void setZ(float z);
		__declspec(property(get = getZ, put = setZ)) float z;

		Point &operator+=(const Vector &v);
		Point &operator-=(const Vector &v);

		friend Point operator+(const Point &P, const Vector &v);
		friend Point operator-(const Point &P, const Vector &v);

		friend Vector operator-(const Point &P, const Point &Q);

		float d(const Point &P) const;   // Distance between two points
		float d2(const Point &P) const;   // Squared distance between two points

		static float d(const Point &P, const Point &Q);   // Distance between two points
		static float d2(const Point &P, const Point &Q);   // Squared distance between two points
	};
}

#include "Vector.h"

namespace MiniBall
{
	inline Point::Point()
	{
	}

	inline Point::Point(const int i)
	{
		const float s = (float)i;

		x = s;
		y = s;
		z = s;
	}

	inline Point::Point(const Point &P)
	{
		x = P.x;
		y = P.y;
		z = P.z;
	}

	inline Point::Point(const Vector &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	inline Point::Point(float P_x, float P_y, float P_z)
	{
		x = P_x;
		y = P_y;
		z = P_z;
	}

	inline Point &Point::operator=(const Point &P)
	{
		x = P.x;
		y = P.y;
		z = P.z;

		return *this;
	}

	inline float Point::getX() const
	{
		return element[0];
	}

	inline void Point::setX(float x_)
	{
		element[0] = x_;
	}

	inline float Point::getY() const
	{
		return element[1];
	}

	inline void Point::setY(float y_)
	{
		element[1] = y_;
	}

	inline float Point::getZ() const
	{
		return element[2];
	}

	inline void Point::setZ(float z_)
	{
		element[2] = z_;
	}
}

#endif   // POINT_H
