#include "stdafx.h"

#include "ViewFrustum.h"

#include "Matrix.h"

namespace MiniBall
{
	ViewFrustum::ViewFrustum()
	{
	}

	ViewFrustum::ViewFrustum(const ViewFrustum &viewFrustum)
	{
		nearPlane = viewFrustum.nearPlane;
		farPlane = viewFrustum.farPlane;
		leftPlane = viewFrustum.leftPlane;
		rightPlane = viewFrustum.rightPlane;
		topPlane = viewFrustum.topPlane;
		bottomPlane = viewFrustum.bottomPlane;
	}

	ViewFrustum::~ViewFrustum()
	{
	}

	ViewFrustum &ViewFrustum::operator=(const ViewFrustum &viewFrustum)
	{
		nearPlane = viewFrustum.nearPlane;
		farPlane = viewFrustum.farPlane;
		leftPlane = viewFrustum.leftPlane;
		rightPlane = viewFrustum.rightPlane;
		topPlane = viewFrustum.topPlane;
		bottomPlane = viewFrustum.bottomPlane;

		return *this;
	}

	ViewFrustum operator*(const Matrix &M, const ViewFrustum &viewFrustum)
	{
		ViewFrustum transformedFrustum;

		transformedFrustum.nearPlane = M * viewFrustum.nearPlane;
		transformedFrustum.farPlane = M * viewFrustum.farPlane;
		transformedFrustum.leftPlane = M * viewFrustum.leftPlane;
		transformedFrustum.rightPlane = M * viewFrustum.rightPlane;
		transformedFrustum.topPlane = M * viewFrustum.topPlane;
		transformedFrustum.bottomPlane = M * viewFrustum.bottomPlane;

		return transformedFrustum;
	}
}