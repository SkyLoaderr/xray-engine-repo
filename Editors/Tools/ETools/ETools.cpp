#include	"stdafx.h"
#include	"ETools.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  fdwReason, 
					  LPVOID lpReserved
					  )
{ 
	switch(fdwReason) {
		case DLL_PROCESS_ATTACH:
			Core._initialize("XRayEditorTools",0,FALSE);
			//FPU::m64r	();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			Core._destroy();
			break;
	}
	return TRUE;
}

namespace ETOOLS{
	ETOOLS_API bool TestRayTriA(const Fvector& C, const Fvector& D, Fvector** p, float& u, float& v, float& range, bool bCull)
	{
		Fvector edge1, edge2, tvec, pvec, qvec;
		float det,inv_det;
		// find vectors for two edges sharing vert0
		edge1.sub(*p[1], *p[0]);
		edge2.sub(*p[2], *p[0]);
		// begin calculating determinant - also used to calculate U parameter
		pvec.crossproduct(D, edge2);
		// if determinant is near zero, ray lies in plane of triangle
		det = edge1.dotproduct(pvec);
		if (bCull){						// define TEST_CULL if culling is desired
			if (det < EPS)  return false;
			tvec.sub(C, *p[0]);							// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec);			// calculate U parameter and test bounds
			if (u < 0.0 || u > det) return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = D.dotproduct(qvec);			// calculate V parameter and test bounds
			if (v < 0.0 || u + v > det) return false;
			range = edge2.dotproduct(qvec);		// calculate t, scale parameters, ray intersects triangle
			inv_det = 1.0f / det;
			range *= inv_det;
			u *= inv_det;
			v *= inv_det;
		}else{											// the non-culling branch
			if (det > -EPS && det < EPS) return false;
			inv_det = 1.0f / det;
			tvec.sub(C, *p[0]);							// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec)*inv_det;	// calculate U parameter and test bounds
			if (u < 0.0f || u > 1.0f)    return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = D.dotproduct(qvec)*inv_det;	// calculate V parameter and test bounds
			if (v < 0.0f || u + v > 1.0f) return false;
			range = edge2.dotproduct(qvec)*inv_det;// calculate t, ray intersects triangle
		}
		return true;
	}
	//-- Ray-Triangle : 1st level of indirection --------------------------------
	ETOOLS_API bool TestRayTriB(const Fvector& C, const Fvector& D, Fvector* p, float& u, float& v, float& range, bool bCull)
	{
		Fvector edge1, edge2, tvec, pvec, qvec;
		float det,inv_det;
		// find vectors for two edges sharing vert0
		edge1.sub(p[1], p[0]);
		edge2.sub(p[2], p[0]);
		// begin calculating determinant - also used to calculate U parameter
		pvec.crossproduct(D, edge2);
		// if determinant is near zero, ray lies in plane of triangle
		det = edge1.dotproduct(pvec);
		if (bCull){						// define TEST_CULL if culling is desired
			if (det < EPS)  return false;
			tvec.sub(C, p[0]);							// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec);			// calculate U parameter and test bounds
			if (u < 0.0f || u > det) return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = D.dotproduct(qvec);			// calculate V parameter and test bounds
			if (v < 0.0f || u + v > det) return false;
			range = edge2.dotproduct(qvec);		// calculate t, scale parameters, ray intersects triangle
			inv_det = 1.0f / det;
			range *= inv_det;
			u *= inv_det;
			v *= inv_det;
		}else{											// the non-culling branch
			if (det > -EPS && det < EPS) return false;
			inv_det = 1.0f / det;
			tvec.sub(C, p[0]);							// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec)*inv_det;	// calculate U parameter and test bounds
			if (u < 0.0f || u > 1.0f)    return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = D.dotproduct(qvec)*inv_det;	// calculate V parameter and test bounds
			if (v < 0.0f || u + v > 1.0f) return false;
			range = edge2.dotproduct(qvec)*inv_det;// calculate t, ray intersects triangle
		}
		return true;
	}
	//-- Ray-Triangle(always return range) : 1st level of indirection --------------------------------
	ETOOLS_API bool TestRayTri2(const Fvector& C, const Fvector& D, Fvector* p, float& range)
	{
		Fvector edge1, edge2, tvec, pvec, qvec;
		float det,inv_det,u,v;

		// find vectors for two edges sharing vert0
		edge1.sub(p[1], p[0]);
		edge2.sub(p[2], p[0]);
		// begin calculating determinant - also used to calculate U parameter
		pvec.crossproduct(D, edge2);
		// if determinant is near zero, ray lies in plane of triangle
		det = edge1.dotproduct(pvec);

		if (_abs(det) < EPS_S)		{ range=-1; return false; }
		inv_det = 1.0f / det;
		tvec.sub(C, p[0]);					// calculate distance from vert0 to ray origin
		u = tvec.dotproduct(pvec)*inv_det;	// calculate U parameter and test bounds
		qvec.crossproduct(tvec, edge1);		// prepare to test V parameter
		range = edge2.dotproduct(qvec)*inv_det;// calculate t, ray intersects plane
		if (u < 0.0f || u > 1.0f)		return false;
		v = D.dotproduct(qvec)*inv_det;		// calculate V parameter and test bounds
		if (v < 0.0f || u + v > 1.0f) return false;
		return true;
	}
}
