#include	"stdafx.h"
#include	"ETools.h"

namespace ETOOLS{
	ETOOLS_API bool TestRayTri(const Fvector& C, const Fvector& D, Fvector** p, float& u, float& v, float& range, bool bCull)
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
	ETOOLS_API bool TestRayTri2(const Fvector& C, const Fvector& D, Fvector* p, float& u, float& v, float& range, bool bCull)
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
}

