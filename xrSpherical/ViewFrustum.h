#ifndef VIEWFRUSTUM_H
#define VIEWFRUSTUM_H

#include "Plane.h"

namespace MiniBall
{
	class Matrix;

	class ViewFrustum
	{
	public:
		Plane nearPlane;
		Plane farPlane;
		Plane leftPlane;
		Plane rightPlane;
		Plane topPlane;
		Plane bottomPlane;

		ViewFrustum();
		ViewFrustum(const ViewFrustum &viewFrustum);

		~ViewFrustum();

		ViewFrustum &operator=(const ViewFrustum &viewFrustum);

		friend ViewFrustum operator*(const Matrix &matrix, const ViewFrustum &viewFrustum);
	};
}

#endif   // VIEWFRUSTUM_H