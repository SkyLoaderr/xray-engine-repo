#include "stdafx.h"
#pragma hdrstop

#include "xrCDB.h"

using namespace CDB;
using namespace Opcode;

IC u32& uf		(float &x) { return (u32&)x; }
IC BOOL	Pick2	(const Fvector& min, const Fvector& max, const Fvector& origin, const Fvector& dir, const Fvector& idir, Fvector& coord)
{
	Fvector				MaxT;
	MaxT.x=MaxT.y=MaxT.z=-1.0f;
	BOOL Inside			= TRUE;
 
	// Find candidate planes.
	if(origin[0] < min[0]) {
		coord[0]	= min[0];
		Inside		= FALSE;
		if(uf(idir[0]))	MaxT[0] = (min[0] - origin[0]) * idir[0]; // Calculate T distances to candidate planes
	} else if(origin[0] > max[0]) {
		coord[0]	= max[0];
		Inside		= FALSE;
		if(uf(idir[0]))	MaxT[0] = (max[0] - origin[0]) * idir[0]; // Calculate T distances to candidate planes
	}
	if(origin[1] < min[1]) {
		coord[1]	= min[1];
		Inside		= FALSE;
		if(uf(idir[1]))	MaxT[1] = (min[1] - origin[1]) * idir[1]; // Calculate T distances to candidate planes
	} else if(origin[1] > max[1]) {
		coord[1]	= max[1];
		Inside		= FALSE;
		if(uf(idir[1]))	MaxT[1] = (max[1] - origin[1]) * idir[1]; // Calculate T distances to candidate planes
	}
	if(origin[2] < min[2]) {
		coord[2]	= min[2];
		Inside		= FALSE;
		if(uf(idir[2]))	MaxT[2] = (min[2] - origin[2]) * idir[2]; // Calculate T distances to candidate planes
	} else if(origin[2] > max[2]) {
		coord[2]	= max[2];
		Inside		= FALSE;
		if(uf(idir[2]))	MaxT[2] = (max[2] - origin[2]) * idir[2]; // Calculate T distances to candidate planes
	}

	// Ray origin inside bounding box
	if(Inside)		{
		coord		= origin;
		return		true;
	}

	// Get largest of the maxT's for final choice of intersection
	u32 WhichPlane = 0;
	if	(MaxT[1] > MaxT[0])				WhichPlane = 1;
	if	(MaxT[2] > MaxT[WhichPlane])	WhichPlane = 2;

	// Check final candidate actually inside box (if max < 0)
	if(uf(MaxT[WhichPlane])&0x80000000) return false;

	if  (0==WhichPlane)	{	// 1 & 2
		coord[1] = origin[1] + MaxT[0] * dir[1];
		if((coord[1] < min[1]) || (coord[1] > max[1]))	return false;
		coord[2] = origin[2] + MaxT[0] * dir[2];
		if((coord[2] < min[2]) || (coord[2] > max[2]))	return false;
		return true;
	}
	if (1==WhichPlane)	{	// 0 & 2
		coord[0] = origin[0] + MaxT[1] * dir[0];
		if((coord[0] < min[0]) || (coord[0] > max[0]))	return false;
		coord[2] = origin[2] + MaxT[1] * dir[2];
		if((coord[2] < min[2]) || (coord[2] > max[2]))	return false;
		return true;
	}
	if (2==WhichPlane)	{	// 0 & 1
		coord[0] = origin[0] + MaxT[2] * dir[0];
		if((coord[0] < min[0]) || (coord[0] > max[0]))	return false;
		coord[1] = origin[1] + MaxT[2] * dir[1];
		if((coord[1] < min[1]) || (coord[1] > max[1]))	return false;
		return true;
	}
	return false;
}

template <bool bCull, bool bFirst, bool bNearest>
class ray_collider
{
public:
	COLLIDER*		dest;
	TRI*			tris;
	Fvector*		verts;
	
	Fvector			rC,rD,rDI;
	float			rRange;
	float			rRange2;

	IC void			_init		(COLLIDER* CL, Fvector* V, TRI* T, const Fvector& C, const Fvector& D, float R)
	{
		dest		= CL;
		tris		= T;
		verts		= V;
		rC.set		(C);
		rD.set		(D);
		rRange		= R;
		rRange2		= R*R;
		if (_abs(rD.x)>flt_eps)	rDI.x= 1.f/rD.x; else rDI.x=0;
		if (_abs(rD.y)>flt_eps)	rDI.y= 1.f/rD.y; else rDI.y=0;
		if (_abs(rD.z)>flt_eps)	rDI.z= 1.f/rD.z; else rDI.z=0;
	}

	IC BOOL			_box		(const Fvector& bCenter, const Fvector& bExtents, Fvector& coord)
	{
		Fbox		BB;
		BB.min.sub	(bCenter,bExtents);
		BB.max.add	(bCenter,bExtents);
        return 		Pick2	(BB.min,BB.max,rC,rD,rDI,coord);
	}
	
	IC bool			_tri		(u32* p, float& u, float& v, float& range)
	{
		Fvector edge1, edge2, tvec, pvec, qvec;
		float	det,inv_det;
		
		// find vectors for two edges sharing vert0
		Fvector&			p0	= verts[ p[0] ];
		Fvector&			p1	= verts[ p[1] ];
		Fvector&			p2	= verts[ p[2] ];
		edge1.sub			(p1, p0);
		edge2.sub			(p2, p0);
		// begin calculating determinant - also used to calculate U parameter
		// if determinant is near zero, ray lies in plane of triangle
		pvec.crossproduct	(rD, edge2);
		det = edge1.dotproduct(pvec);
		if (bCull)
		{						
			if (det < EPS)  return false;
			tvec.sub(rC, p0);						// calculate distance from vert0 to ray origin
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
			tvec.sub(rC, p0);						// calculate distance from vert0 to ray origin
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
					R.id		= prim;
					R.range		= r;
					R.u			= u;
					R.v			= v;
					R.verts	[0]	= verts[tris[prim].verts[0]];
					R.verts	[1]	= verts[tris[prim].verts[1]];
					R.verts	[2]	= verts[tris[prim].verts[2]];
					R.dummy		= tris[prim].dummy;
					rRange		= r;
					rRange2		= r*r;
				}
			} else {
				RESULT& R	= dest->r_add();
				R.id		= prim;
				R.range		= r;
				R.u			= u;
				R.v			= v;
				R.verts	[0]	= verts[tris[prim].verts[0]];
				R.verts	[1]	= verts[tris[prim].verts[1]];
				R.verts	[2]	= verts[tris[prim].verts[2]];
				R.dummy		= tris[prim].dummy;
				rRange		= r;
				rRange2		= r*r;
			}
		} else {
			RESULT& R	= dest->r_add();
			R.id		= prim;
			R.range		= r;
			R.u			= u;
			R.v			= v;
			R.verts	[0]	= verts[tris[prim].verts[0]];
			R.verts	[1]	= verts[tris[prim].verts[1]];
			R.verts	[2]	= verts[tris[prim].verts[2]];
			R.dummy		= tris[prim].dummy;
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
	m_def->syncronize		();

	// Get nodes
	const AABBNoLeafTree* T = (const AABBNoLeafTree*)m_def->tree->GetTree();
	const AABBNoLeafNode* N = T->GetNodes();
	r_clear					();
	
	// Binary dispatcher
	if (ray_mode&OPT_CULL)
	{
		if (ray_mode&OPT_ONLYFIRST)
		{
			if (ray_mode&OPT_ONLYNEAREST)
			{
				ray_collider<true,true,true>	RC;
				RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
				RC._stab(N);
			} else {
				ray_collider<true,true,false>	RC;
				RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
				RC._stab(N);
			}
		} else {
			if (ray_mode&OPT_ONLYNEAREST)
			{
				ray_collider<true,false,true>	RC;
				RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
				RC._stab(N);
			} else {
				ray_collider<true,false,false>	RC;
				RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
				RC._stab(N);
			}
		}
	} else {
		if (ray_mode&OPT_ONLYFIRST)
		{
			if (ray_mode&OPT_ONLYNEAREST)
			{
				ray_collider<false,true,true>	RC;
				RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
				RC._stab(N);
			} else {
				ray_collider<false,true,false>	RC;
				RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
				RC._stab(N);
			}
		} else {
			if (ray_mode&OPT_ONLYNEAREST)
			{
				ray_collider<false,false,true>	RC;
				RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
				RC._stab(N);
			} else {
				//__asm int 3;
				ray_collider<false,false,false>	RC;
				RC._init(this,m_def->verts,m_def->tris,r_start,r_dir,r_range);
				RC._stab(N);
			}
		}
	}
}

