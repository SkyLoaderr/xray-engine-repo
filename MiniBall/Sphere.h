#ifndef SPHERE_H
#define SPHERE_H

#include "Point.h"

#include <math.h>

namespace MiniBall
{
	class Vector;
	class Matrix;

	class Sphere
	{
	public:
		Point center;
		float radius;

		Sphere();
		Sphere(const Sphere &X);
		Sphere(const Point &O);   // Point-Sphere
		Sphere(const Point &O, float R);   // Center and radius (not squared)
		Sphere(const Point &O, const Point &A);   // Sphere through two points
		Sphere(const Point &O, const Point &A, const Point &B);   // Sphere through three points
		Sphere(const Point &O, const Point &A, const Point &B, const Point &C);   // Sphere through four points

		Sphere &operator=(const Sphere &S);

		float d(const Point &P) const;  // Distance from p to boundary of the Sphere
		float d2(const Point &P) const;  // Square distance from p to boundary of the Sphere

		static float d(const Sphere &S, const Point &P);  // Distance from p to boundary of the Sphere
		static float d(const Point &P, const Sphere &S);  // Distance from p to boundary of the Sphere

		static float d2(const Sphere &S, const Point &P);  // Square distance from p to boundary of the Sphere
		static float d2(const Point &P, const Sphere &S);  // Square distance from p to boundary of the Sphere

		static Sphere miniBall(Point P[], unsigned int p);		// Smallest enclosing sphere
		static Sphere miniBall_Ptr(Point** P, unsigned int p);  // Smallest enclosing sphere
		static Sphere smallBall(Point P[], unsigned int p);		// Enclosing sphere approximation
		static Sphere goodBall(Point P[], unsigned int p);		// Enclosing sphere approximation
		static Sphere dualBall(Point P[], unsigned int p);		// Enclosing sphere approximation
	private:
		static Sphere recurseMini(Point *P[], unsigned int p, unsigned int b = 0);
	};
}

#endif   // SPHERE_H

