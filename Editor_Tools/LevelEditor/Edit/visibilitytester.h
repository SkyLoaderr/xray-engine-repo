#ifndef _VIS_TEST_
#define _VIS_TEST_
#pragma once

enum EFC_Planes {
	fcpLeft	= 0,
	fcpRight,
	fcpBottom,
	fcpTop,
//	fcpNear,
	fcpFar,
	fcp_last,
	fcp_forcedword = DWORD(-1)
};
enum EFC_Visible {
	fcvNone = 0,
	fcvPartial,
	fcvFully,
	fcv_forcedword = DWORD(-1)
};

// planes are directed outside
class CFrustumClipper {
	Fplane			Planes	[fcp_last];	// planes of the frustum
	Fvector			ProjDirs[fcp_last]; // directions of projectors along corners of frustum
	Fvector			COP;				// center-of-projection for the frustum (world coords)
	float			wR,wL,wT,wB;		// viewplane windows extents on z=1 camera coord plane
private:
	int				frustumEdgeIsect(Fvector& Start, Fvector& End);
	int				obbOverlapPlane	(Fplane &P, Fvector *obbV, Fvector &X, Fvector &Y, Fvector &Z);
	int				obbRayIsect		(Fvector &Start, Fvector &Dir,
									Fvector* obbV, Fvector &X, Fvector &Y, Fvector &Z,
									float &InT, float &OutT);
	int				obbOverlapVF	(Fvector* V, Fvector &X, Fvector &Y, Fvector &Z);
public:
	float			fFarPlane;

	void			Projection		(float fAspect, float FOV, float N, float F);

	void			BuildFrustum	(Fvector &P, Fvector &D, Fvector &N);
	void			DrawFrustum		();

	// Sphere visibility
	EFC_Visible		visibleSphere	(Fvector &c, float r);

	// Oriented bounding box visibility
	EFC_Visible		visibleOBB		();

	// Axis-aligned bounding box visibility
	EFC_Visible		visibleAABB		();
};


#endif
