#include "stdafx.h"
#include "xrCDB.h"

using namespace CDB;
using namespace Opcode;

template <bool bCull, bool bFirst, bool bNearest>
class ray_collider
{
public:
	COLLIDER*		dest;
	TRI*			tris;
	
	Fvector			rC,rD;
	float			rRange;
	float			rRange2;

	IC void			_init		(COLLIDER* CL, TRI* T, const Fvector& C, const Fvector& D, float R)
	{
		dest		= CL;
		tris		= T;
		rC.set		(C);
		rD.set		(D);
		rRange		= R;
		rRange2		= R*R;
	}

	IC BOOL			_box		(const Fvector& bCenter, const Fvector& bExtents, Fvector& coord)
	{
		Fbox		BB;
		BB.min.sub	(bCenter,bExtents);
		BB.max.add	(bCenter,bExtents);
        return 		BB.Pick2(rC,rD,coord);
		
		/*
		BOOL		Inside = TRUE;
		Fvector		MaxT,bMin,bMax;
		MaxT.set	(-1.f,-1.f,-1.f);
		bMin.sub	(bCenter,bExtents);
		bMax.add	(bCenter,bExtents);
		
		// Find candidate planes.
		if(rC[0] < bMin[0]) {
			Inside		= FALSE;	coord[0]	= bMin[0];
			MaxT[0]		= (bMin[0] - rC[0]) / rD[0];	// Calculate T distances to candidate planes
		} else if(rC[0] > bMax[0]) {
			Inside		= FALSE;	coord[0]	= bMax[0];
			MaxT[0]		= (bMax[0] - rC[0]) / rD[0];	// Calculate T distances to candidate planes
		}
		if(rC[1] < bMin[1]) {
			Inside		= FALSE;	coord[1]	= bMin[1];
			MaxT[1]		= (bMin[1] - rC[1]) / rD[1];	// Calculate T distances to candidate planes
		} else if(rC[1] > bMax[1]) {
			Inside		= FALSE;	coord[1]	= bMax[1];
			MaxT[1]		= (bMax[1] - rC[1]) / rD[1];	// Calculate T distances to candidate planes
		}
		if(rC[2] < bMin[2]) {
			Inside		= FALSE;	coord[2]	= bMin[2];
			MaxT[2]		= (bMin[2] - rC[2]) / rD[2];	// Calculate T distances to candidate planes
		} else if(rC[2] > bMax[2]) {
			Inside		= FALSE;	coord[2]	= bMax[2];
			MaxT[2]		= (bMax[2] - rC[2]) / rD[2];	// Calculate T distances to candidate planes
		}
		
		// Ray rP inside bounding box
		if(Inside)		{ coord.set	(rC); return true; }
		
		// Get largest of the maxT's for final choice of intersection
		DWORD WhichPlane = 0;
		if(MaxT[1] > MaxT[0])			WhichPlane = 1;
		if(MaxT[2] > MaxT[WhichPlane])	WhichPlane = 2;
		
		// Check final candidate actually inside box
		if(IR(MaxT[WhichPlane])&0x80000000) return false;
		
		switch (WhichPlane) {
		case 0:	// 1 & 2
			coord[1] = rC[1] + MaxT[0] * rD[1];				// 1 1 0 1
			if(fabsf(coord[1]) > bMax[1])	return false;
			coord[2] = rC[2] + MaxT[0] * rD[2];				// 2 2 0 2
			if(fabsf(coord[2]) > bMax[2])	return false;
			return true;
		case 1:	// 0 & 2
			coord[0] = rC[0] + MaxT[1] * rD[0];				// 0 0 1 0
			if(fabsf(coord[0]) > bMax[0])	return false;
			coord[2] = rC[2] + MaxT[1] * rD[2];				// 2 2 1 2
			if(fabsf(coord[2]) > bMax[2])	return false;
			return true;
		case 2:	// 0 & 1
			coord[0] = rC[0] + MaxT[2] * rD[0];				// 0 0 2 0
			if(fabsf(coord[0]) > bMax[0])	return false;
			coord[1] = rC[1] + MaxT[2] * rD[1];				// 1 1 2 1
			if(fabsf(coord[1]) > bMax[1])	return false;
			return true;
		default:
			NODEFAULT;
#ifdef DEBUG
			return false;
#endif
		}
		*/
	}
	
	IC bool			_tri		(Fvector** p, float& u, float& v, float& range)
	{
		Fvector edge1, edge2, tvec, pvec, qvec;
		float det,inv_det;
		
		// find vectors for two edges sharing vert0
		edge1.sub			(*p[1], *p[0]);
		edge2.sub			(*p[2], *p[0]);
		// begin calculating determinant - also used to calculate U parameter
		pvec.crossproduct	(rD, edge2);
		// if determinant is near zero, ray lies in plane of triangle
		det = edge1.dotproduct(pvec);
		if (bCull)
		{						
			if (det < EPS)  return false;
			tvec.sub(rC, *p[0]);						// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec);					// calculate U parameter and test bounds
			if (u < 0.f || u > det) return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = rD.dotproduct(qvec);					// calculate V parameter and test bounds
			if (v < 0.f || u + v > det) return false;
			range = edge2.dotproduct(qvec);				// calculate t, scale parameters, ray intersects triangle
			inv_det = 1.0f / det;
			range	*= inv_det;
			u		*= inv_det;
			v		*= inv_det;
		}
		else
		{			
			if (det > -EPS && det < EPS) return false;
			inv_det = 1.0f / det;
			tvec.sub(rC, *p[0]);						// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec)*inv_det;			// calculate U parameter and test bounds
			if (u < 0.0f || u > 1.0f)    return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = rD.dotproduct(qvec)*inv_det;			// calculate V parameter and test bounds
			if (v < 0.0f || u + v > 1.0f) return false;
			range = edge2.dotproduct(qvec)*inv_det;		// calculate t, ray intersects triangle
		}
		return true;
	}
	
	void			_prim		(DWORD prim)
	{
		float	u,v,r;
		if (!_tri(tris[prim].verts, u, v, r))	return;
		if (r<=0 || r>rRange)					return;
		
		if (bNearest)	
		{
			if (dest->r_count())	
			{
				RESULT& R = *dest->r_begin();
				if (r<R.range)	{
					R.id	= prim;
					R.range	= r;
					R.u		= u;
					R.v		= v;
					rRange	= r;
					rRange2	= r*r;
				}
			} else {
				RESULT& R	= dest->r_add();
				R.id		= prim;
				R.range		= r;
				R.u			= u;
				R.v			= v;
				rRange		= r;
				rRange2		= r*r;
			}
		} else {
			RESULT& R	= dest->r_add();
			R.id		= prim;
			R.range		= r;
			R.u			= u;
			R.v			= v;
		}
	}
	void			_stab		(const AABBNoLeafNode* node)
	{
		Fvector	P;
		
		// Actual ray/aabb test
		if (!_box((Fvector&)node->mAABB.mCenter,(Fvector&)node->mAABB.mExtents,P))	return;
		if (P.distance_to_sqr(rC)>rRange2)											return;
		
		// 1st chield
		if (node->HasLeaf())	_prim	(node->GetPrimitive());
		else					_stab	(node->GetPos());
		
		// Early exit for "only first"
		if (bFirst && dest->r_count())												return;
		
		// 2nd chield
		if (node->HasLeaf2())	_prim	(node->GetPrimitive2());
		else					_stab	(node->GetNeg());
	}
};

void	COLLIDER::ray_query	(const MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range)
{
	// Get nodes
	const AABBNoLeafTree* T = (const AABBNoLeafTree*)m_def->tree->GetTree();
	const AABBNoLeafNode* N = T->GetNodes();
	r_clear	();
	
	// Binary dispatcher
	if (ray_mode&OPT_CULL)
	{
		if (ray_mode&OPT_ONLYFIRST)
		{
			if (ray_mode&OPT_ONLYNEAREST)
			{
				ray_collider<true,true,true>	RC;
				RC._init(this,m_def->tris,r_start,r_dir,r_range);
				RC._stab(N);
			} else {
				ray_collider<true,true,false>	RC;
				RC._init(this,m_def->tris,r_start,r_dir,r_range);
				RC._stab(N);
			}
		} else {
			if (ray_mode&OPT_ONLYNEAREST)
			{
				ray_collider<true,false,true>	RC;
				RC._init(this,m_def->tris,r_start,r_dir,r_range);
				RC._stab(N);
			} else {
				ray_collider<true,false,false>	RC;
				RC._init(this,m_def->tris,r_start,r_dir,r_range);
				RC._stab(N);
			}
		}
	} else {
		if (ray_mode&OPT_ONLYFIRST)
		{
			if (ray_mode&OPT_ONLYNEAREST)
			{
				ray_collider<false,true,true>	RC;
				RC._init(this,m_def->tris,r_start,r_dir,r_range);
				RC._stab(N);
			} else {
				ray_collider<false,true,false>	RC;
				RC._init(this,m_def->tris,r_start,r_dir,r_range);
				RC._stab(N);
			}
		} else {
			if (ray_mode&OPT_ONLYNEAREST)
			{
				ray_collider<false,false,true>	RC;
				RC._init(this,m_def->tris,r_start,r_dir,r_range);
				RC._stab(N);
			} else {
				ray_collider<false,false,false>	RC;
				RC._init(this,m_def->tris,r_start,r_dir,r_range);
				RC._stab(N);
			}
		}
	}
}
