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
class ENGINE_API CFrustumClipper {
	Fplane			Planes	[fcp_last];	// planes of the frustum
	Fvector			ProjDirs[fcp_last]; // directions of projectors along corners of frustum
	Fvector			COP;				// center-of-projection for the frustum (world coords)
	float			wR,wL,wT,wB;		// viewplane windows extents on z=1 camera coord plane
public:
	float			fFarPlane;

	void			Projection		(float FOV, float N, float F);

	void			BuildFrustum	(Fvector &P, Fvector &D, Fvector &N);

	// Sphere visibility
	IC EFC_Visible	visibleSphere	(Fvector &c, float r)
	{
		float		cls1,cls2,cls3,cls4,cls5;
		// left
		cls1 = Planes[fcpLeft].classify(c);
		if (cls1>r) return fcvNone;
		
		// right
		cls2 = Planes[fcpRight].classify(c);
		if (cls2>r) return fcvNone;
		
		// bottom
		cls3 = Planes[fcpBottom].classify(c);
		if (cls3>r) return fcvNone;
		
		// top
		cls4 = Planes[fcpTop].classify(c);
		if (cls4>r) return fcvNone;
		
		// far
		cls5 = Planes[fcpFar].classify(c);
		if (cls5>r) return fcvNone;
		
		// now we have detected that sphere is visible
		// test for 'partial' visibility...
		if (fabsf(cls1)<r || fabsf(cls2)<r || fabsf(cls3)<r || fabsf(cls4)<r || fabsf(cls5)<r)
			return fcvPartial;
		else
			return fcvFully;
	}
};


#endif
