#include "stdafx.h"

#include "Plane.h"

#include "Matrix.h"

#include <math.h>

namespace MiniBall
{
	Plane &Plane::operator+()
	{
		return *this;
	}

	Plane Plane::operator-() const
	{
		return Plane(-n, -D);
	}

	Plane &Plane::operator*=(const Matrix &M)
	{
		return *this = *this * M;
	}

	Plane operator*(const Plane &p, const Matrix &T)
	{
		Matrix M = ~T^-1;

		return Plane(p.A * M(1, 1) + p.B * M(2, 1) + p.C * M(3, 1) + p.D * M(4, 1),
		             p.A * M(1, 2) + p.B * M(2, 2) + p.C * M(3, 2) + p.D * M(4, 2),
		             p.A * M(1, 3) + p.B * M(2, 3) + p.C * M(3, 3) + p.D * M(4, 3),
		             p.A * M(1, 4) + p.B * M(2, 4) + p.C * M(3, 4) + p.D * M(4, 4));
	}

	Plane operator*(const Matrix &T, const Plane &p)
	{
		Matrix M = ~T^-1;

		return Plane(M(1, 1) * p.A + M(1, 2) * p.B + M(1, 3) * p.C + M(1, 4) * p.D,
		             M(2, 1) * p.A + M(2, 2) * p.B + M(2, 3) * p.C + M(2, 4) * p.D,
		             M(3, 1) * p.A + M(3, 2) * p.B + M(3, 3) * p.C + M(3, 4) * p.D,
		             M(4, 1) * p.A + M(4, 2) * p.B + M(4, 3) * p.C + M(4, 4) * p.D);
	}

	float operator^(const Plane &p1, const Plane &p2)
	{
		return acosf(p1.n * p2.n / (N(p1.n) * N(p2.n)));
	}

	float Plane::d(const Point &P) const
	{
		return P * n + D;
	}

	float Plane::d(const Point &P, const Plane &p)
	{
		return P * p.n + p.D;
	}

	float Plane::d(const Plane &p, const Point &P)
	{
		return p.n * P + p.D;
	}

	Plane &Plane::normalise()
	{
		float l = N(n);

		n /= l;
		D /= l;

		return *this;
	}
}
