#ifndef BOUNDINGSPHERE_H
#define BOUNDINGSPHERE_H

#include "BoundingVolume.h"
#include "Sphere.h"

namespace MiniBall
{
	class Matrix;
	class ViewFrustum;
	class Viewport;
	class Plane;

	class BoundingSphere : public BoundingVolume, public Sphere
	{
	public:
		BoundingSphere();
		BoundingSphere(const BoundingSphere &B);
		BoundingSphere(const Sphere &S);

		~BoundingSphere();

		BoundingSphere &operator=(const BoundingSphere &B);
		BoundingSphere &operator=(const Sphere &B);

		bool visible(const ViewFrustum &Frustum) const;
		bool intersect(const Plane &ClippingPlane) const;

		bool frontSide(const Plane &ClippingPlane) const;
		bool backSide(const Plane &ClippingPlane) const;
	}; 
}

#endif   // BOUNDINGSPHERE_H