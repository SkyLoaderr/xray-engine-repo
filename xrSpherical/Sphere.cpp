#include "stdafx.h"

#include "Sphere.h"

#include "Vector.h"
#include "Matrix.h"

#include "stdlib.h"

namespace MiniBall
{
	Sphere::Sphere()
	{
		radius = -1;
	}

	Sphere::Sphere(const Sphere &S)
	{
		radius = S.radius;
		center = S.center;
	}

	Sphere::Sphere(const Point &O)
	{
		radius = 0;
		center = O;
	}

	Sphere::Sphere(const Point &O, float R)
	{
		radius = R;
		center = O;
	}

	Sphere::Sphere(const Point &O, const Point &A)
	{
		Vector a = A - O;

		Vector o = 0.5f * a;

		radius = N(o);
		center = O + o;
	}

	Sphere::Sphere(const Point &O, const Point &A,  const Point &B)
	{
		Vector a = A - O;
		Vector b = B - O;

		float Denominator = 2.0f * ((a % b) * (a % b));

		Vector o = ((b^2) * ((a % b) % a) +
		            (a^2) * (b % (a % b))) / Denominator;

		radius = N(o);
		center = O + o;
	}

	Sphere::Sphere(const Point &O, const Point &A, const Point &B, const Point &C)
	{
		Vector a = A - O;
		Vector b = B - O;
		Vector c = C - O;

		float Denominator = 2.0f * Matrix::det(a.x, a.y, a.z,
		                                       b.x, b.y, b.z,
		                                       c.x, c.y, c.z);

		Vector o = ((c^2) * (a % b) +
		            (b^2) * (c % a) +
		            (a^2) * (b % c)) / Denominator;

		radius = N(o);
		center = O + o;
	}

	Sphere &Sphere::operator=(const Sphere &S)
	{
		radius = S.radius;
		center = S.center;

		return *this;
	}

	float Sphere::d(const Point &P) const
	{
		return Point::d(P, center) - radius;
	}

	float Sphere::d2(const Point &P) const
	{
		return Point::d2(P, center) - radius*radius;
	}

	float Sphere::d(const Sphere &S, const Point &P)
	{
		return Point::d(P, S.center) - S.radius;
	}

	float Sphere::d(const Point &P, const Sphere &S)
	{
		return Point::d(P, S.center) - S.radius;
	}

	float Sphere::d2(const Sphere &S, const Point &P)
	{
		return Point::d2(P, S.center) - S.radius*S.radius;
	}

	float Sphere::d2(const Point &P, const Sphere &S)
	{
		return Point::d2(P, S.center) - S.radius*S.radius;
	}

	Sphere Sphere::recurseMini(Point *P[], unsigned int p, unsigned int b)
	{
		Sphere MB;

		switch(b)
		{
		case 0:
			MB = Sphere();
			break;
		case 1:
			MB = Sphere(*P[-1]);
			break;
		case 2:
			MB = Sphere(*P[-1], *P[-2]);
			break;
		case 3:
			MB = Sphere(*P[-1], *P[-2], *P[-3]);
			break;
		case 4:
			MB = Sphere(*P[-1], *P[-2], *P[-3], *P[-4]);
			return MB;
		}

		for(unsigned int i = 0; i < p; i++)
			if(MB.d2(*P[i]) > 0)   // Signed square distance to sphere
			{
				for(unsigned int j = i; j > 0; j--)
				{
					Point *T = P[j];
					P[j] = P[j - 1];
					P[j - 1] = T;
				}

				MB = recurseMini(P + 1, i, b + 1);
			}

		return MB;
	}

	Sphere Sphere::miniBall(Point P[], unsigned int p)
	{
		Point **L = new Point*[p];

		for(unsigned int i = 0; i < p; i++)
			L[i] = &P[i];

		Sphere MB = recurseMini(L, p);

		delete[] L;

		return MB;
	}
	Sphere Sphere::miniBall_Ptr(Point** P, unsigned int p)
	{
		return recurseMini(P, p);
	}

	Sphere Sphere::smallBall(Point P[], unsigned int p)
	{
		Vector center;
		float radius = -1;

		if(p > 0)
		{
			center = 0;

			for(unsigned int i = 0; i < p; i++)
				center += P[i];

			center /= (float)p;

			for(i = 0; i < p; i++)
			{
				float d2 = Point::d2(P[i], center);
			
				if(d2 > radius)
					radius = d2;
			}

			radius = sqrtf(radius);
		}

		return Sphere(center, radius);
	}


	inline void _min(float &val, float &modifier)
	{	if (modifier<val) val=modifier;	}
	inline void _max(float &val, float &modifier)
	{	if (modifier>val) val=modifier;	}
	Sphere Sphere::goodBall(Point P[], unsigned int p)
	{
		Vector Center, Min=P[0], Max=P[0];
		float  radius = -1;

		for (unsigned int i=0; i<p; i++) {
			_min(Min.element[0],P[i].element[0]);_max(Max.element[0],P[i].element[0]);
			_min(Min.element[1],P[i].element[1]);_max(Max.element[1],P[i].element[1]);
			_min(Min.element[2],P[i].element[2]);_max(Max.element[2],P[i].element[2]);
		}
		Center = (Min+Max)/2.f;

		for(i = 0; i < p; i++)
		{
			float d2 = Point::d2(P[i], Center);
			if(d2 > radius)
				radius = d2;
		}
		radius = sqrtf(radius);

		return Sphere(Center, radius);
	}
	Sphere Sphere::dualBall(Point P[], unsigned int p)
	{
		unsigned int i;

		// 1.
		Vector Min=P[0], Max=P[0];
		Vector C1=0,C2=0;
		for (i=0; i<p; i++) {
			_min(Min.element[0],P[i].element[0]);_max(Max.element[0],P[i].element[0]);
			_min(Min.element[1],P[i].element[1]);_max(Max.element[1],P[i].element[1]);
			_min(Min.element[2],P[i].element[2]);_max(Max.element[2],P[i].element[2]);
			C2 += P[i];
		}
		C1 = (Min+Max)/2.f;
		C2 /= (float)p;

		// R.
		Vector Center	= (C1+C2)/2;
		float radius	= -1;
		for(i = 0; i < p; i++)
		{
			float d2 = Point::d2(P[i], Center);
			if(d2 > radius)	radius = d2;
		}
		radius = sqrtf(radius);
		return Sphere(Center, radius);
	}
}
