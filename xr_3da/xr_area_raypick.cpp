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
BOOL CObjectSpace::RayTest	( const Fvector &start, const Fvector &dir, float range, Collide::rq_target tgt, Collide::ray_cache* cache)
{
	VERIFY					(_abs(dir.magnitude()-1)<EPS);

	r_results.r_clear		();

	xrc.ray_options			(CDB::OPT_ONLYFIRST);
	Collide::ray_defs Q		(start,dir,range,CDB::OPT_ONLYFIRST,tgt);
	if (tgt&rqtDynamic) 
	{
		// Traverse object database
		g_SpatialSpace->q_ray	(0,STYPE_COLLIDEABLE,start,dir,range);

		// Determine visibility for dynamic part of scene
		for (u32 o_it=0; o_it<g_SpatialSpace->q_result.size(); o_it++)
		{
			ISpatial*	spatial			= g_SpatialSpace->q_result[o_it];
			CObject*	collidable		= dynamic_cast<CObject*>			(spatial);
			if (collidable && collidable->collidable.model->_RayQuery(Q,r_results))	return TRUE;
		}
	}
	
	if (tgt&rqtStatic){
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
	return FALSE;
}

BOOL CObjectSpace::RayPick	( const Fvector &start, const Fvector &dir, float range, rq_target tgt, rq_result& R)
{
	r_temp.r_clear		();
	R.O		= 0; R.range = range; R.element = -1;
	if (tgt&rqtStatic){ 
		// Test static model
		xrc.ray_options		(CDB::OPT_ONLYNEAREST | CDB::OPT_CULL);
		xrc.ray_query		(&Static,start,dir,range);
		if (xrc.r_count())  R.set_if_less(xrc.r_begin());
	}

	if (tgt&rqtDynamic){ 
		Collide::ray_defs Q		(start,dir,R.range,CDB::OPT_ONLYNEAREST|CDB::OPT_CULL,tgt);
		// Traverse object database
		g_SpatialSpace->q_ray	(0,STYPE_COLLIDEABLE,start,dir,range);

		// Determine visibility for dynamic part of scene
		for (u32 o_it=0; o_it<g_SpatialSpace->q_result.size(); o_it++)
		{
			ISpatial*	spatial			= g_SpatialSpace->q_result[o_it];
			CObject*	collidable		= dynamic_cast<CObject*>	(spatial);
			if			(0==collidable)	continue;
			u32		C	= D3DCOLOR_XRGB	(64,64,64);
			Q.range		= R.range;
			if (collidable->collidable.model->_RayQuery(Q,r_temp))	
			{
				C	= D3DCOLOR_XRGB(128,128,196);
				R.set_if_less(r_temp.r_begin());
			}
			if (bDebug) 
			{
				Fsphere	S;		S.P = spatial->spatial.center; S.R = spatial->spatial.radius;
				dbg_S.push_back	(mk_pair(S,C));
			}
		}
	}
	return (R.element>=0);
}

BOOL CObjectSpace::RayQuery(const Collide::ray_defs& R, Collide::rq_callback* CB, LPVOID user_data)
{
	// initialize query
	r_results.r_clear	();
	r_temp.r_clear		();

	Flags32		sd_test;	sd_test.set		(R.tgt);
	rq_target	next_test	= R.tgt;

	rq_result	s_res;
	ray_defs	s_rd(R.start,R.dir,R.range,CDB::OPT_ONLYNEAREST|CDB::OPT_CULL,R.tgt);
	ray_defs	d_rd(R.start,R.dir,R.range,CDB::OPT_ONLYNEAREST|CDB::OPT_CULL,R.tgt);

	s_res.set				(0,s_rd.range,-1);
	do{
		if ((R.tgt&rqtStatic)&&sd_test.is(rqtStatic)&&(next_test&rqtStatic)){ 
			s_res.set		(0,s_rd.range,-1);
			// Test static model
			xrc.ray_options	(s_rd.flags);
			xrc.ray_query	(&Static,s_rd.start,s_rd.dir,s_rd.range);
			if (xrc.r_count()){	
				if (s_res.set_if_less(xrc.r_begin())){
					s_rd.range	-=	(s_res.range+EPS_L);
					s_rd.start.mad	(s_rd.dir,s_res.range+EPS_L);
					s_res.range	= R.range-s_rd.range-EPS_L;
				}
			}
			if (!s_res.valid())	sd_test.set(rqtStatic,FALSE);
		}
		if ((R.tgt&rqtDynamic)&&sd_test.is(rqtDynamic)&&(next_test&rqtDynamic)){ 
			r_temp.r_clear	();
			// Traverse object database
			g_SpatialSpace->q_ray	(0,STYPE_COLLIDEABLE,d_rd.start,d_rd.dir,d_rd.range);
			// Determine visibility for dynamic part of scene
			for (u32 o_it=0; o_it<g_SpatialSpace->q_result.size(); o_it++){
				CObject*	collidable		= dynamic_cast<CObject*>	(g_SpatialSpace->q_result[o_it]);
				if			(0==collidable)	continue;
				ICollisionForm*	cform		= collidable->collidable.model;
				if (cform->_RayQuery(d_rd,r_temp)){
					rq_result& d_res = *r_temp.r_begin();
					d_rd.range	-= (d_res.range+EPS_L);
					d_rd.start.mad(d_rd.dir,d_res.range+EPS_L);
					d_res.range	= R.range-d_rd.range-EPS_L;
				}
			}
			if (!r_temp.r_count()) sd_test.set(rqtDynamic,FALSE);
		}
		if (s_res.valid()&&r_temp.r_count()){
			if	(s_res.range<r_temp.r_begin()->range){
				BOOL need_calc	= CB?CB(s_res,user_data):TRUE;
				next_test		= need_calc?rqtStatic:rqtNone;
				r_results.append_result	(s_res);
			}else{
				BOOL need_calc	= CB?CB(*r_temp.r_begin(),user_data):TRUE;
				next_test		= need_calc?rqtDynamic:rqtNone;	
				r_results.append_result	(*r_temp.r_begin());
			}
		}else if (s_res.valid()){
			BOOL need_calc		= CB?CB(s_res,user_data):TRUE;
			next_test			= need_calc?rqtStatic:rqtNone;
			r_results.append_result		(s_res);
		}else if (r_temp.r_count()){
			BOOL need_calc		= CB?CB(*r_temp.r_begin(),user_data):TRUE;
			next_test			= need_calc?rqtDynamic:rqtNone;
			r_results.append_result		(*r_temp.r_begin());
		}else{
			next_test			= rqtNone;
		}
		if ((R.flags&CDB::OPT_ONLYFIRST)||(R.flags&CDB::OPT_ONLYNEAREST)) break;
	}while(next_test!=rqtNone);
	return r_results.r_count();
}

BOOL CObjectSpace::RayQuery(ICollisionForm* target, const Collide::ray_defs& R)
{
	VERIFY(target);
	r_results.r_clear();
	return target->_RayQuery(R,r_results);
}

/*
BOOL CObjectSpace::RayPick( rq_target tgt, ray_query& Q, rq_callback* CB )
{
	const Fvector& dir		= Q._dir		();
	Flags32		sd_test;	sd_test.set		(tgt);
	rq_target	next_test	= tgt;

	rq_result	s_res;
	float		s_range		= Q._range();
	Fvector		s_start		= Q._start();
	rq_result	d_res;
	float		d_range		= Q._range();
	Fvector		d_start		= Q._start();

	s_res.set				(0,s_range,-1);
	d_res.set				(0,d_range,-1);

	do{
		if ((tgt&rqtStatic)&&sd_test.is(rqtStatic)&&(next_test&rqtStatic)){ 
			s_res.set		(0,s_range,-1);
			// Test static model
			xrc.ray_options	(CDB::OPT_ONLYNEAREST | CDB::OPT_CULL);
			xrc.ray_query	(&Static,s_start,dir,s_range);
			if (xrc.r_count()){	
				if (s_res.set_if_less(xrc.r_begin())){
					s_range		-= (s_res.range+EPS_L);
					s_start.mad (dir,s_res.range+EPS_L);
					s_res.range	= Q._range()-s_range-EPS_L;
				}
			}
			if (!s_res.valid())	sd_test.set(rqtStatic,FALSE);
		}
		if ((tgt&rqtDynamic)&&sd_test.is(rqtDynamic)&&(next_test&rqtDynamic)){ 
			d_res.set		(0,d_range,-1);
			// Traverse object database
			g_SpatialSpace->q_ray	(0,STYPE_COLLIDEABLE,d_start,dir,d_range);
			// Determine visibility for dynamic part of scene
			for (u32 o_it=0; o_it<g_SpatialSpace->q_result.size(); o_it++){
				CObject*	collidable		= dynamic_cast<CObject*>	(g_SpatialSpace->q_result[o_it]);
				if			(0==collidable)	continue;
				ICollisionForm*	cform		= collidable->collidable.model;
				ray_query	r_temp(d_start,dir,d_range,CDB::OPT_ONLYNEAREST|CDB::OPT_CULL);
				if (cform->_RayQuery(r_temp)){
					if (d_res.set_if_less(r_temp.r_begin())){
						d_range		-= (d_res.range+EPS_L);
						d_start.mad (dir,d_res.range+EPS_L);
						d_res.range	= Q._range()-d_range-EPS_L;
					}
				}
			}
			if (!d_res.valid()) sd_test.set(rqtDynamic,FALSE);
		}
		if (s_res.valid()&&d_res.valid()){
			if	(s_res.range<d_res.range){
				BOOL need_calc	= CB?CB(s_res):TRUE;
				next_test		= need_calc?rqtStatic:rqtNone;
				Q.append_result	(s_res);
			}else{
				BOOL need_calc	= CB?CB(d_res):TRUE;
				next_test		= need_calc?rqtDynamic:rqtNone;	
				Q.append_result	(d_res);
			}
		}else if (s_res.valid()){
			BOOL need_calc		= CB?CB(s_res):TRUE;
			next_test			= need_calc?rqtStatic:rqtNone;
			Q.append_result		(s_res);
		}else if (d_res.valid()){
			BOOL need_calc		= CB?CB(d_res):TRUE;
			next_test			= need_calc?rqtDynamic:rqtNone;
			Q.append_result		(d_res);
		}else{
			next_test			= rqtNone;
		}
	}while(next_test!=rqtNone);
	return Q.r_count();
}
/*/
