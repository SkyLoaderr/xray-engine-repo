#include "stdafx.h"
#include "xr_area.h"
#include "xr_collide_form.h"
#include "xr_object.h"
#include "collide\cl_intersect.h"

#include "xr_creator.h"
#include "x_ray.h"
#include "xr_smallfont.h"


using namespace	Collide;

//----------------------------------------------------------------------
static int test_cnt = 0;
static int slot_cnt = 0;
static int nl_cnt = 0;

BOOL CObjectSpace::nl_append(int x, int z, const Fvector2& O, const Fvector2& D)
{
	if(bDebug) nl_cnt++;
	if ((x<0)||(z<0)||(x>=Dynamic.x_count)||(z>=Dynamic.z_count)) return false;

	SLOT& S = Dynamic(x,z); 
	if(dwQueryID!=S.dwQueryID){
		S.dwQueryID = dwQueryID;
		if(bDebug) test_cnt++;
		if(TestRaySlot(x,z,O,D)){
			if(bDebug){
				slot_cnt++;
				pApp->pFont->OutNext("{%d,%d}",x,z);
				dbg_Slot.push_back(Fvector2());
				dbg_Slot.back().set(float(x),float(z));
			}
	
			for(DWORD I=0; I<S.lst.size(); I++) {
				CCFModel* M = S.lst[I]->CFORM();
				if(M->GetEnable() && M->dwQueryID!=dwQueryID){
					M->dwQueryID=dwQueryID;
					nearest_list.push_back(M);
				}
			}
		}
	}

	return true;
}
//----------------------------------------------------------------------
BOOL CObjectSpace::TestRaySlot(int x, int z, const Fvector2& start, const Fvector2& dir){
	Fvector2 center; InvTrans(center,x,z);
	Fbox2 B;
	B.min.sub(center,CL_SLOT_SIZE/2);
	B.max.add(center,CL_SLOT_SIZE/2);
	return !!B.Pick(start,dir);
}
//----------------------------------------------------------------------

void CObjectSpace::CaptureSlots(const Fvector& start, const Fvector& dir, float range)
{
	dbg_Slot.clear();

	if (bDebug) pApp->pFont->OutSet(0.5f,-1.f);
	slot_cnt=0;
	test_cnt=0;
	nl_cnt=0;

	Fvector end; end.direct(start,dir,range);
	Fvector2 S,D;
	S.set(start.x,start.z);
	D.set(dir.x,dir.z); D.norm_safe();

	int x1	= TransX(start.x);
	int z1	= TransZ(start.z);
	int x2	= TransX(end.x);
	int z2	= TransZ(end.z);
	
    int dx	= abs(x2 - x1);
    int dz	= abs(z2 - z1);
    int sx	= x2 >= x1 ? 1 : -1;
    int sz	= z2 >= z1 ? 1 : -1;
	
	if((dx==0)&&(dz==0)){
//		pApp->pFont->Out(-1.f,0.6f,"Variant 0");
		nl_append(x1,z1,S,D);
	}else if( dz <= dx ){
//		pApp->pFont->Out(-1.f,0.6f,"Variant X");
        int d  = ( dz << 1 ) - dx;
        int d1 = dz << 1;
        int d2 = ( dz - dx ) << 1;
		
		nl_append(x1,z1,S,D);
		nl_append(x1,z1+sz,S,D);
//		nl_append(x1,z1-sz,S,D);

		for(int x=x1+sx, z=z1, i=1; i<=dx; i++, x+=sx){
			if ( d > 0)	{d+=d2; z+=sz;}
			else         d+=d1;
			nl_append(x,z+sz,S,D);
			nl_append(x,z-sz,S,D);
			nl_append(x+sx,z-sz,S,D);
			if (!nl_append(x,z,S,D)) break;
		}
	}else{
//		pApp->pFont->Out(-1.f,0.6f,"Variant Z");
		int d  = ( dx << 1 ) - dz;
		int d1 = dx << 1;
		int d2 = ( dx - dz ) << 1;
		
		nl_append(x1,z1,S,D);
		nl_append(x1+sx,z1,S,D);
//		nl_append(x1-sx,z1,S,D);
		
		for(int x=x1, z=z1+sz, i=1; i<=dz; i++, z+=sz){
			if ( d > 0){d+=d2; x+=sx;}
			else		d += d1;
			nl_append(x+sx,z,S,D);
			nl_append(x-sx,z,S,D);
			nl_append(x-sx,z+sz,S,D);
			if (!nl_append(x,z,S,D)) break;
		}
	}
	if(bDebug)
		pApp->pFont->Out(-1.f,0.55f,"NL: %d, Test: %d, Slot: %d", nl_cnt, test_cnt, slot_cnt);
}
//--------------------------------------------------------------------------------
// Occluded/No
BOOL CObjectSpace::RayTest( const Fvector &start, const Fvector &dir, float range, BOOL bDynamic, Collide::ray_cache* cache)
{
	VERIFY(fabsf(dir.magnitude()-1)<EPS);

	XRC.RayMode			(RAY_ONLYFIRST);
	CCFModel::RayQuery	Q;
	Q.start = start; Q.dir = dir; Q.range=range; Q.element=0;
	if (bDynamic) 
	{
		// Trace RAY through grid
		dwQueryID			++;
		nearest_list.clear	( );
		CaptureSlots	(start,dir,range);
		
		// Iterate on objects
		for ( NL_IT nl_idx=nearest_list.begin(); nl_idx!=nearest_list.end(); nl_idx++ )
		{
			CCFModel&	M = *(*nl_idx);
										  
			if (M.Owner()->bVisible) 
			{
				if (M._svRayTest(Q))	return TRUE;
			}
		}
	}
	
	// If we get here - test static model
	if (cache) {
		// 1. Check cached polygon
		float _u,_v,_range;
		if (RAPID::TestRayTri(start,dir,cache->verts,_u,_v,_range,false)) 
		{
			if (_range>0 && _range<range) return TRUE;
		}
		
		// 2. Polygon doesn't pick - real database query
		XRC.RayPick(0,&Static,start,dir,Q.range);
		if (0==XRC.GetRayContactCount()) {
			return FALSE;
		} else {
			// cache polygon
			RAPID::raypick_info&	rpinf	= XRC.RayContact[0];
			cache->verts[0].set	(rpinf.p[0]);
			cache->verts[1].set	(rpinf.p[1]);
			cache->verts[2].set	(rpinf.p[2]);
			return TRUE;
		}
	} else {
		XRC.RayPick(0,&Static,start,dir,Q.range);
		return XRC.GetRayContactCount();
	}
}

BOOL CObjectSpace::RayPick( const Fvector &start, const Fvector &dir, float range, ray_query& R)
{
	dwQueryID++;

	CCFModel::RayQuery	Q;
	Q.start = start; Q.dir = dir; Q.range=range; Q.element=0;
	R.O		= 0; R.range = range; R.element = -1;

	// Test static model
	XRC.RayMode			(RAY_ONLYNEAREST|RAY_CULL);
	XRC.RayPick			(0,&Static,Q.start,Q.dir,Q.range);
	const RAPID::raypick_info* I = XRC.GetMinRayPickInfo();
	if (I) {
		if (I->range<range) {
			R.O			= NULL;
			R.range		= I->range;
			Q.range		= I->range;
			R.element	= I->id;
		}
	}

	// Trace RAY through grid
	nearest_list.clear	( );
	CaptureSlots		(Q.start,Q.dir,Q.range);
	
	// Iterate on objects
	for ( NL_IT nl_idx=nearest_list.begin(); nl_idx!=nearest_list.end(); nl_idx++ )
	{
		CCFModel&	M = *(*nl_idx);
		
		DWORD C = D3DCOLOR_XRGB(64,64,64);
		if (M._svRayTest(Q)) {
			C = D3DCOLOR_XRGB(128,128,196);
			if (Q.range<R.range) {
				R.O			= M.owner;
				R.range		= Q.range;
				R.element	= Q.element;
			}
		}
		if (bDebug) dbg_S.push_back(make_pair(M.GetSphere(),C));
	}
	return (R.element>=0);
}

//--------------------------------------------------------------------------------
// выбор треугольников которые пересек box
// заполняется q_tris и collide_list, если передается range, то вычисляется min dist 
//--------------------------------------------------------------------------------
/*
#define MIN_DIST_FROM(s) dist = (d-N.dotproduct((s)))/NDir; if ((dist>0)&&(dist<fDist)) fDist=dist;
BOOL  CObjectSpace::RayPickW( const Fvector &start, const Fvector &dir, float range, float width, clQueryRay& R)
{
	// setup matrix
	Fmatrix M_bbox;
	D.normalize			(dir);
	N.normalize			(norm);
	R.crossproduct		(D,N);	R.normalize();
	M_bbox.set			(R,N,D,start);
	
	// Trace RAY through grid
	dwQueryID++;
	nearest_list.clear	( );
	Fvector				end;
	end.direct			(start,dir,range);
	FillLine			(TransX(start.x), TransZ(start.z), TransX(end.x), TransZ(end.z) );
	
	// Iterate on objects
	XRC.RayMode			(RAY_ONLYNEAREST|RAY_CULL);
	CCFModel::RayQuery	Q;
	Q.start = start; Q.dir = dir; Q.range=range; Q.element=0;
	R.O = 0; R.range = range; R.element = -1;
	
	for ( NL_IT nl_idx=nearest_list.begin(); nl_idx!=nearest_list.end(); nl_idx++ )
	{
		CCFModel&	M = *(*nl_idx);
		
		Fvector C; float _R; M.GetSphere	(C,_R);
		if (!RAPID::IntersectRaySphere(start,dir,C,_R+width)) continue;
		
		if (M._RayTest(Q)) {
			if (Q.range<R.range) {
				R.O			= M.owner;
				R.range		= Q.range;
				R.element	= Q.element;
			}
		}
	}
	
	Fvector				box_center,box_radius;
	CCFModel*			result	= NULL;
	int					id		= 0;
	
	// set bbox params
	box_radius.set		(w/2,h/2,max_range/2);
	box_center.set		(0,0,box_radius.z);
	
	Fvector start1,start2,start3,start4;
	start1.set( box_radius.x, box_radius.y,0); M_bbox.transform_tiny(start1);
	start2.set(-box_radius.x,-box_radius.y,0); M_bbox.transform_tiny(start2);
	start3.set( box_radius.x,-box_radius.y,0); M_bbox.transform_tiny(start3);
	start4.set(-box_radius.x, box_radius.y,0); M_bbox.transform_tiny(start4);
	// set raypick mode
	XRC.BBoxMode(BBOX_TRITEST|(bFirstContact?BBOX_ONLYFIRST:0)|(bCulling?BBOX_CULL:0));
	
	// test models
	for ( NL_IT nl_idx=nearest_list.begin(); nl_idx!=nearest_list.end(); nl_idx++ ){
		target			= *nl_idx;
		Fmatrix &M_target= target->owner?target->owner->mTransform:precalc_identity;
		// check collide
		XRC.BBoxCollide	(M_target,&target->model, M_bbox,box_center,box_radius);
		if (!XRC.GetBBoxContactCount()) continue;
		// fill tris list
		Fmatrix *pM = (target->owner)?&(target->owner->mTransform):0;
		for (DWORD i=0; i<XRC.GetBBoxContactCount(); i++){
												AddToTrisList( pM, &target->model.tris[XRC.BBoxContact[i].id] );
												if (range){
													clQueryTri& t=q_tris[q_tris.size()-1];
													Fvector N; N.mknormal(t.p[0],t.p[1],t.p[2]);
													float dist, fDist = max_range;
													float u,v;
													RAPID::TestRayTri(M_bbox.c, M_bbox.k,t.p,u,v,dist,bCulling); if ((dist>0)&&(dist<fDist)) fDist=dist;
													RAPID::TestRayTri(start1,	M_bbox.k,t.p,u,v,dist,bCulling); if ((dist>0)&&(dist<fDist)) fDist=dist;
													RAPID::TestRayTri(start2,	M_bbox.k,t.p,u,v,dist,bCulling); if ((dist>0)&&(dist<fDist)) fDist=dist;
													RAPID::TestRayTri(start3,	M_bbox.k,t.p,u,v,dist,bCulling); if ((dist>0)&&(dist<fDist)) fDist=dist;
													RAPID::TestRayTri(start4,	M_bbox.k,t.p,u,v,dist,bCulling); if ((dist>0)&&(dist<fDist)) fDist=dist;
													if (fDist>0){
														if (!range || bFirstContact){
															if (range) *range = fDist;
															collide_list.push_back( target );
															return		q_tris.size();
														}else{
															if ( fDist < *range ){
																*range	= fDist;
																result	= target;
																id		=XRC.BBoxContact[i].id;
															}
														}
													}
												}
		}
	}
	if (result){
		q_tris.clear();
		collide_list.push_back( result );
		Fmatrix *pM = (result->owner)?&(result->owner->mTransform):0;
		AddToTrisList( pM, &result->model.tris[id] );
	}
	
	//------------------------------------------------------------
	XRC.BBoxCollide	(precalc_identity,&Static, M_bbox,box_center,box_radius);
	if (!XRC.GetBBoxContactCount()) return 0;
	
	collide_list.push_back(0);
	
	// fill tris list
	for (DWORD i=0; i<XRC.GetBBoxContactCount(); i++)
	{
		RAPID::tri &T = Static.tris[XRC.BBoxContact[i].id];
		if (range){
			Fvector N; N.mknormal(*T.verts[0],*T.verts[1],*T.verts[2]);
			float dist, fDist = max_range;
			float u,v;
			RAPID::TestRayTri(M_bbox.c, M_bbox.k,T.verts,u,v,dist,bCulling); if ((dist>0)&&(dist<fDist)) fDist=dist;
			RAPID::TestRayTri(start1,	M_bbox.k,T.verts,u,v,dist,bCulling); if ((dist>0)&&(dist<fDist)) fDist=dist;
			RAPID::TestRayTri(start2,	M_bbox.k,T.verts,u,v,dist,bCulling); if ((dist>0)&&(dist<fDist)) fDist=dist;
			RAPID::TestRayTri(start3,	M_bbox.k,T.verts,u,v,dist,bCulling); if ((dist>0)&&(dist<fDist)) fDist=dist;
			RAPID::TestRayTri(start4,	M_bbox.k,T.verts,u,v,dist,bCulling); if ((dist>0)&&(dist<fDist)) fDist=dist;
			if (fDist>0)
			{
				if (bFirstContact)
				{
					*range = fDist;
					AddToTrisList(&T);
					return		q_tris.size();
				}else{
					if ( fDist < *range ){
						*range	= fDist;
						id		=XRC.BBoxContact[i].id;
					}
				}
			}
		}
	}
	
	AddToTrisList(&Static.tris[id]);
	return q_tris.size();
														}
														BOOL  CObjectSpace::BoxPick( const Fvector &start, const Fvector &dir, const Fvector &norm, float w, float h, float* range, bool bFirstContact, bool bCulling ){
														Fvector D,N,R;
														Fmatrix T;
														// set parent matrix
														D.normalize			(dir);
														N.normalize			(norm);
														R.crossproduct		(D,N);	R.normalize();
														T.set				(R,N,D,start);
														
														  return BoxPick		(T,w,h,range,bFirstContact,bCulling);
														  }
														  
															BOOL  CObjectSpace::BoxPick( const Fvector &start, const Fvector &dir, float radius, float* range, bool bFirstContact, bool bCulling ){
															Fvector D,N,R;
															Fmatrix T;
															D.normalize			(dir);
															if(1-fabsf(D.y)<EPS)N.set(1,0,0);
															else            	N.set(0,1,0);
															R.crossproduct		(N,D);	R.normalize();
															N.crossproduct		(D,R);	N.normalize();
															T.set				(R,N,D,start);
															return BoxPick		(T,radius,radius,range,bFirstContact,bCulling);
															}
															
															  #define RIGHT	0
															  #define LEFT	1
															  #define MIDDLE	2
															  
																bool HitBoundingBox(const Fvector2& minB, const Fvector2& maxB, const Fvector2& origin, const Fvector2& dir, Fvector2& coord)
																{
																BOOL inside = TRUE;
																BOOL quadrant[2];
																int i;
																int whichPlane;
																float maxT[2];
																float candidatePlane[2];
																
																  // Find candidate planes; this loop can be avoided if
																  //rays cast all from the eye(assume perpsective view)
																  for (i=0; i<2; i++){
																  if(origin[i]<minB[i]) {
																  quadrant[i] = LEFT;
																  candidatePlane[i] = minB[i];
																  inside = FALSE;
																  }else if (origin[i]>maxB[i]) {
																  quadrant[i] = RIGHT;
																  candidatePlane[i] = maxB[i];
																  inside = FALSE;
																  }else	{
																  quadrant[i] = MIDDLE;
																  }
																  }
																  
																	// Ray origin inside bounding box
																	if(inside)	{
																	coord.set(origin);
																	return true;
																	}
																	
																	  
																		// Calculate T distances to candidate planes
																		for (i = 0; i < 2; i++)
																		if (quadrant[i] != MIDDLE && dir[i] !=0.)
																		maxT[i] = (candidatePlane[i]-origin[i]) / dir[i];
																		else
																		maxT[i] = -1.;
																		
																		  // Get largest of the maxT's for final choice of intersection
																		  whichPlane = 0;
																		  for (i = 1; i < 2; i++)
																		  if (maxT[whichPlane] < maxT[i])
																		  whichPlane = i;
																		  
																			// Check final candidate actually inside box
																			if (maxT[whichPlane] < 0.) return false;
																			for (i = 0; i < 2; i++)
																			if (whichPlane != i) {
																			coord[i] = origin[i] + maxT[whichPlane] *dir[i];
																			if (coord[i] < minB[i]) || coord[i] > maxB[i]))
																			return false;	// outside box
																			}else {
																			coord[i] = candidatePlane[i];
																			}
																			return true;				// ray hits box
																			}	
*/
