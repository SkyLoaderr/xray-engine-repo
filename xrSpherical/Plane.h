#ifndef PLANE_H
#define PLANE_H

#include "Vector.h"

namespace MiniBall
{
	class Matrix;
	class Point;

	class Plane
	{
	public:
		union
		{
			struct
			{
				float A;
				float B;
				float C;
			};
			struct
			{
				Vector n;
			};
		};

		float D;   // Distance to origin along normal

		Plane();
		Plane(const Plane &p);
		Plane(const Vector &n, float D);   // Normal and distance to origin
		Plane(const Vector &n, const Point &P);   // Normal and point on plane
		Plane(const Point &P0, const Point &P1, const Point &P2);   // Through three points
		Plane(float A, float B, float C, float D);   // Plane equation 

		Plane &operator=(const Plane &p);

		Plane &operator+();
		Plane operator-() const;   // Flip normal

		Plane &operator*=(const Matrix &A);   // Transform plane by matrix (post-multiply)

		friend Plane operator*(const Plane &p, const Matrix &A);   // Transform plane by matrix (post-multiply)
		friend Plane operator*(const Matrix &A, const Plane &p);   // Transform plane by matrix (pre-multiply)

		friend float operator^(const Plane &p1, const Plane &p2);   // Angle between planes

		float d(const Point &P) const;   // Oriented distance between point and plane

		static float d(const Point &P, const Plane &p);   // Oriented distance between point and plane
		static float d(const Plane &p, const Point &P);   // Oriented distance between plane and point

		Plane &normalise();   // Normalise the Plane equation
	};
}

#include "Point.h"

namespace MiniBall
{
	inline Plane::Plane()
	{
	}

	inline Plane::Plane(const Plane &p)
	{
		n = p.n;
		D = p.D;
	}

	inline Plane::Plane(const Vector &p_n, float p_D)
	{
		n = p_n;
		D = p_D;
	}

	inline Plane::Plane(const Vector &p_n, const Point &P)
	{
		n = p_n;
		D = -n * P;
	}

	inline Plane::Plane(const Point &P0, const Point &P1, const Point &P2)
	{
		n = (P1 - P0) % (P2 - P0);
		D = -n * P0;
	}

	inline Plane::Plane(float p_A, float p_B, float p_C, float p_D)
	{
		A = p_A;
		B = p_B;
		C = p_C;
		D = p_D;
	}

	inline Plane &Plane::operator=(const Plane &p)
	{
		n = p.n;
		D = p.D;

		return *this;
	}
}

#endif   // PLANE_H
