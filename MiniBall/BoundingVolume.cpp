#include "stdafx.h"

#include "BoundingVolume.h"

#include "ViewFrustum.h"
#include "Plane.h"

namespace MiniBall
{
	bool BoundingVolume::visible(const ViewFrustum &viewFrustum) const
	{
		// NOTE: Conservative test, not exact

		return !backSide(viewFrustum.nearPlane) &&
		       !backSide(viewFrustum.farPlane) &&
		       !backSide(viewFrustum.leftPlane) &&
		       !backSide(viewFrustum.rightPlane) &&
		       !backSide(viewFrustum.topPlane) &&
		       !backSide(viewFrustum.bottomPlane);
	}

	bool BoundingVolume::intersect(const Plane &plane) const
	{
		return !frontSide(plane) && !backSide(plane);
	}

	bool BoundingVolume::backSide(const Plane &plane) const
	{
		return frontSide(-plane);
	}
}
