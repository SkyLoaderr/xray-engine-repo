#include "stdafx.h"
#include "xr_area.h"
#include "xr_collide_form.h"
#include "xr_object.h"
#include "cl_intersect.h"

#include "igame_level.h"
#include "x_ray.h"
#include "GameFont.h"

using namespace	Collide;

//--------------------------------------------------------------------------------
// Occluded/No
BOOL CObjectSpace::RayTest	( const Fvector &start, const Fvector &dir, float range, BOOL bDynamic, Collide::ray_cache* cache)
{
	VERIFY					(_abs(dir.magnitude()-1)<EPS);

	xrc.ray_options			(CDB::OPT_ONLYFIRST);
	Collide::RayQuery Q		(start,dir,range,CDB::OPT_ONLYFIRST);
	if (bDynamic) 
	{
		// Traverse object database
		g_SpatialSpace->q_ray	(0,STYPE_COLLIDEABLE,start,dir,range);

		// Determine visibility for dynamic part of scene
		for (u32 o_it=0; o_it<g_SpatialSpace->q_result.size(); o_it++)
		{
			ISpatial*	spatial			= g_SpatialSpace->q_result[o_it];
			CObject*	collidable		= dynamic_cast<CObject*>			(spatial);
			if (collidable && collidable->collidable.model->_RayTest(Q))	return TRUE;
		}
	}
	
	// If we get here - test static model
	if (cache) 
	{
		// 1. Check cached polygon
		float _u,_v,_range;
		if (CDB::TestRayTri(start,dir,cache->verts,_u,_v,_range,false)) 
		{
			if (_range>0 && _range<range) return TRUE;
		}
		
		// 2. Polygon doesn't pick - real database query
		xrc.ray_query	(&Static,start,dir,range);
		if (0==xrc.r_count()) {
			return FALSE;
		} else {
			// cache polygon
			CDB::RESULT*	R	= xrc.r_begin();
			CDB::TRI&		T	= Static.get_tris() [ R->id ];
			Fvector*		V	= Static.get_verts();
			cache->verts[0].set	(V[T.verts[0]]);
			cache->verts[1].set	(V[T.verts[1]]);
			cache->verts[2].set	(V[T.verts[2]]);
			return TRUE;
		}
	} else {
		xrc.ray_query	(&Static,start,dir,range);
		return xrc.r_count();
	}
}

BOOL CObjectSpace::RayPick	( const Fvector &start, const Fvector &dir, float range, rq_result& R)
{
	R.O		= 0; R.range = range; R.element = -1;

	// Test static model
	xrc.ray_options		(CDB::OPT_ONLYNEAREST | CDB::OPT_CULL);
	xrc.ray_query		(&Static,start,dir,range);
	if (xrc.r_count()) {
		CDB::RESULT* I	= xrc.r_begin	();
		if (I->range<range) {
			R.O			= NULL;
			R.range		= I->range;
			R.element	= I->id;
		}
	}

	// Traverse object database
	g_SpatialSpace->q_ray	(0,STYPE_COLLIDEABLE,start,dir,range);

	// Determine visibility for dynamic part of scene
	for (u32 o_it=0; o_it<g_SpatialSpace->q_result.size(); o_it++)
	{
		ISpatial*	spatial			= g_SpatialSpace->q_result[o_it];
		CObject*	collidable		= dynamic_cast<CObject*>	(spatial);
		if			(0==collidable)	continue;
		u32		C	= D3DCOLOR_XRGB	(64,64,64);
		Collide::RayQuery Q			(start,dir,R.range,CDB::OPT_ONLYNEAREST|CDB::OPT_CULL);
		if (collidable->collidable.model->_RayTest(Q))	
		{
			Collide::rq_result* res = Q.r_begin(); 
			C	= D3DCOLOR_XRGB(128,128,196);
			if (res->range<R.range) R = *res;
		}
		if (bDebug) 
		{
			Fsphere	S;		S.P = spatial->spatial.center; S.R = spatial->spatial.radius;
			dbg_S.push_back	(mk_pair(S,C));
		}
	}

	return (R.element>=0);
}
