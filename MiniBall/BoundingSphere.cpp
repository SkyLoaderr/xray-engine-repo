#include "stdafx.h"

#include "BoundingSphere.h"
#include "ViewFrustum.h"
#include "Plane.h"

namespace MiniBall
{
	BoundingSphere::BoundingSphere()
	{
	}

	BoundingSphere::BoundingSphere(const BoundingSphere &boundingSphere)
	{
		center = boundingSphere.center;
		radius = boundingSphere.radius;
	}

	BoundingSphere::BoundingSphere(const Sphere &sphere)
	{
		center = sphere.center;
		radius = sphere.radius;
	}

	BoundingSphere &BoundingSphere::operator=(const BoundingSphere &boundingSphere)
	{
		center = boundingSphere.center;
		radius = boundingSphere.radius;

		return *this;
	}

	BoundingSphere &BoundingSphere::operator=(const Sphere &sphere)
	{
		center = sphere.center;
		radius = sphere.radius;

		return *this;
	}

	BoundingSphere::~BoundingSphere()
	{
	}

	bool BoundingSphere::visible(const ViewFrustum &viewFrustum) const
	{
		// NOTE: Conservative test, not exact

		if(Plane::d(viewFrustum.nearPlane, center) < -radius)
			return false;

		if(Plane::d(viewFrustum.farPlane, center) < -radius)
			return false;

		if(Plane::d(viewFrustum.leftPlane, center) < -radius)
			return false;

		if(Plane::d(viewFrustum.rightPlane, center) < -radius)
			return false;

		if(Plane::d(viewFrustum.topPlane, center) < -radius)
			return false;

		if(Plane::d(viewFrustum.bottomPlane, center) < -radius)
			return false;

		return true;
	}

	bool BoundingSphere::intersect(const Plane &plane) const
	{
		return fabs(Plane::d(plane, center)) < radius;
	}

	bool BoundingSphere::frontSide(const Plane &plane) const
	{
		return Plane::d(plane, center) > radius;
	}

	bool BoundingSphere::backSide(const Plane &plane) const
	{
		return Plane::d(plane, center) < -radius;
	}
}