#ifndef D_SORT_TRI_PRIMITIVE_H
#define D_SORT_TRI_PRIMITIVE_H
#include "dTriCollideK.h"
#include "dTriColliderCommon.h"
#include "dTriColliderMath.h"
#include "__aabb_tri.h"
#include "../MathUtils.h"
#ifdef DEBUG
#include "../../StatGraph.h"
#include "../PHDebug.h"

#endif

static 	xr_vector<Triangle> 		pos_tries			;
static 	xr_vector<Triangle> 		neg_tries			;
		xr_vector< flags8 >			gl_cl_tries_state	;
		xr_vector<int>::iterator	I,E,B				;
	
void VxToGlClTriState(u32 v)
{
	CDB::TRI*       T_array      = Level().ObjectSpace.GetStaticTris();
	xr_vector<int>::iterator LI=I+1;
	for(;E!=LI;++LI)
	{
		u32* verts=T_array[*LI].verts;
		flags8 &state  =gl_cl_tries_state[LI-B];
		if(verts[0]==v)state.set(fl_engaged_v0,TRUE);
		if(verts[1]==v)state.set(fl_engaged_v1,TRUE);
		if(verts[2]==v)state.set(fl_engaged_v2,TRUE);
	}
}
void SideToGlClTriState(u32 v0,u32 v1)
{
	CDB::TRI*       T_array      = Level().ObjectSpace.GetStaticTris();
	xr_vector<int>::iterator LI=I+1;
	for(;E!=LI;++LI)
	{
		u32* verts=T_array[*LI].verts;
		flags8 &state  =gl_cl_tries_state[LI-B];
		if(verts[0]==v1&&verts[1]==v0)state.set(fl_engaged_s0,TRUE);
		if(verts[1]==v1&&verts[2]==v0)state.set(fl_engaged_s1,TRUE);
		if(verts[2]==v1&&verts[0]==v0)state.set(fl_engaged_s2,TRUE);
	}
}


template<class T>
int dSortTriPrimitiveCollide (	
							  dxGeom		*o1,		dxGeom			*o2,
							  int			flags,		dContactGeom	*contact,	int skip,
							  const Fvector&	AABB
							  )
{

	

	dxGeomUserData* data=dGeomGetUserData(o1);
	dReal* last_pos=data->last_pos;
	bool	no_last_pos	=last_pos[0]==-dInfinity;
	const dReal* p=dGeomGetPosition(o1);

	Fbox last_box;last_box.setb(data->last_aabb_pos,data->last_aabb_size);
	Fbox box;box.setb(cast_fv(p),AABB);
	

	CDB::TRI*       T_array                         = Level().ObjectSpace.GetStaticTris();
	const Fvector*	 V_array						 = Level().ObjectSpace.GetStaticVerts();
	if(no_last_pos||!last_box.contains(box))
	{
		
		Fvector aabb;aabb.set(AABB);
		aabb.mul(ph_tri_query_ex_aabb_rate);
	///////////////////////////////////////////////////////////////////////////////////////////////
		XRC.box_options                (0);
		XRC.box_query                  (Level().ObjectSpace.GetStaticModel(),cast_fv(p),aabb);

		CDB::RESULT*    R_begin                         = XRC.r_begin()	;
		CDB::RESULT*    R_end                           = XRC.r_end()	;
#ifdef DEBUG
		
		dbg_total_saved_tries-=data->cashed_tries.size();
		dbg_new_queries_per_step++;
#endif
		data->cashed_tries								.clear()		;
		for (CDB::RESULT* Res=R_begin; Res!=R_end; ++Res)
		{
			data->cashed_tries.push_back(Res->id);
		}
#ifdef DEBUG
		dbg_total_saved_tries+=data->cashed_tries.size();
#endif
		data->last_aabb_pos.set(cast_fv(p));
		data->last_aabb_size.set(aabb);
	}
#ifdef DEBUG
	else
		dbg_reused_queries_per_step++;
#endif
///////////////////////////////////////////////////////////////////////////////////////////////	
	int			ret	=	0;

	pos_tries.clear	();
	dReal neg_depth=dInfinity,b_neg_depth=dInfinity;
	UINT	b_count		=0			;
	bool	intersect	=	false	;

#ifdef DEBUG
	if(ph_dbg_draw_mask.test(phDbgDrawTriTestAABB))
		DBG_DrawAABB(cast_fv(p),AABB,D3DCOLOR_XRGB(0,0,255));
#endif


	bool* pushing_neg=&data->pushing_neg;
	bool* pushing_b_neg=&data->pushing_b_neg;
	bool spushing_neg=*pushing_neg;
	bool spushing_b_neg=*pushing_b_neg;
	Triangle neg_tri;//=&(data->neg_tri);
	Triangle b_neg_tri;//=&(data->b_neg_tri);
	if(*pushing_neg){
		CalculateTri(data->neg_tri,p,neg_tri,V_array);
		if(neg_tri.dist<0.f)
		{
			dReal sidePr=T::Proj(o1,neg_tri.norm);
			neg_tri.depth=sidePr-neg_tri.dist;
			neg_depth=neg_tri.depth;
		}
		else
		{
			*pushing_neg=false;
		}
	
#ifdef DEBUG
		if(ph_dbg_draw_mask.test(phDbgDrawSavedTries))
			DBG_DrawTri(neg_tri.T,V_array,D3DCOLOR_XRGB(255,0,0));
#endif

	}

	if(*pushing_b_neg){
		CalculateTri(data->b_neg_tri,p,b_neg_tri,V_array);
		if(b_neg_tri.dist<0.f)
		{
			dReal sidePr=T::Proj(o1,b_neg_tri.norm);
			b_neg_tri.depth=sidePr-b_neg_tri.dist;
			b_neg_depth=b_neg_tri.depth;
		}
		else
		{
			*pushing_b_neg=false;
		}

#ifdef DEBUG
		if(ph_dbg_draw_mask.test(phDbgDrawSavedTries))
			DBG_DrawTri(b_neg_tri.T,V_array,D3DCOLOR_XRGB(0,0,255));
#endif

		}

	bool b_pushing=*pushing_neg||*pushing_b_neg;
	gl_cl_tries_state.resize(data->cashed_tries.size(),Flags8().assign(0));
	B=data->cashed_tries.begin(),E=data->cashed_tries.end();
	for (I=B; I!=E; ++I)
	{
#ifdef DEBUG
		dbg_saved_tries_for_active_objects++;
#endif
		//if(ignored_tries[I-B])continue;
		CDB::TRI* T = T_array + *I;
		const Point vertices[3]={Point((dReal*)&V_array[T->verts[0]]),Point((dReal*)&V_array[T->verts[1]]),Point((dReal*)&V_array[T->verts[2]])};
		if(!aabb_tri_aabb(Point(p),Point((float*)&AABB),vertices))continue;
#ifdef DEBUG
		if(ph_dbg_draw_mask.test(phDBgDrawIntersectedTries))
										DBG_DrawTri(T,V_array,D3DCOLOR_XRGB(0,255,0));
		dbg_tries_num++;
#endif
		Triangle	tri;	
		CalculateTri(T,p,tri,vertices);
		if(tri.dist<0.f){
#ifdef DEBUG
			if(ph_dbg_draw_mask.test(phDBgDrawNegativeTries))
				DBG_DrawTri(T,V_array,D3DCOLOR_XRGB(0,0,255));
#endif
			float last_pos_dist=dDOT(last_pos,tri.norm)-tri.pos;
			if((!(last_pos_dist<0.f))||b_pushing)
				if(__aabb_tri(Point(p),Point((float*)&AABB),vertices))
				{
#ifdef DEBUG
					if(ph_dbg_draw_mask.test(phDBgDrawTriesChangesSign))
						DBG_DrawTri(T,V_array,D3DCOLOR_XRGB(0,255,0));
#endif
					if(!b_pushing)
					{
						if(!no_last_pos)
						{
#ifdef DEBUG
							if(ph_dbg_draw_mask.test(phDbgDrawTriTrace))
								DBG_DrawLine(cast_fv(last_pos),cast_fv(p),D3DCOLOR_XRGB(255,0,255));
#endif
							dVector3 tri_point;
							PlanePoint(tri,last_pos,p,last_pos_dist,tri_point);
#ifdef DEBUG
							if(ph_dbg_draw_mask.test(phDbgDrawTriPoint))
								DBG_DrawPoint(cast_fv(tri_point),0.01f,D3DCOLOR_XRGB(255,0,255));
#endif
							intersect=intersect||TriContainPoint(	
								vertices[0],
								vertices[1],
								vertices[2],
								tri.norm,tri.side0,
								tri.side1,tri_point);
						}
						else
						{
							intersect=true;
						}
					}
					else
					{
						intersect=true;
					}

					if(TriContainPoint(
						vertices[0],
						vertices[1],
						vertices[2],
						tri.norm,tri.side0,
						tri.side1,p)

						){
							dReal sidePr=T::Proj(o1,tri.norm);
							tri.depth=sidePr-tri.dist;
							if(neg_depth>tri.depth&&(!(*pushing_neg||spushing_neg)||dDOT(neg_tri.norm,tri.norm)>-M_SQRT1_2)&&(!(*pushing_b_neg||spushing_b_neg)||dDOT(b_neg_tri.norm,tri.norm)>-M_SQRT1_2))//exclude switching on opposite side &&(!*pushing_b_neg||dDOT(b_neg_tri->norm,tri.norm)>-M_SQRT1_2)
							{
								neg_depth=tri.depth;
								neg_tri=tri;
								data->neg_tri=tri.T;
								ret=0;
								//if(intersect)*pushing_neg=true;
							}


						}
					else{
						++b_count;
						dReal sidePr=T::Proj(o1,tri.norm);
						tri.depth=sidePr-tri.dist;
						if(b_neg_depth>tri.depth&&(!(*pushing_b_neg||spushing_b_neg)||dDOT(b_neg_tri.norm,tri.norm)>-M_SQRT1_2)&&((!*pushing_neg||!spushing_neg)||dDOT(neg_tri.norm,tri.norm)>-M_SQRT1_2)){//exclude switching on opposite side &&(!*pushing_neg||dDOT(neg_tri->norm,tri.norm)>-M_SQRT1_2)
							b_neg_depth=tri.depth;
							b_neg_tri=tri;
							data->b_neg_tri=tri.T;
							ret=0;
							//if(intersect)*pushing_b_neg=true;
						}
					}
				}
		}
		else{
#ifdef DEBUG
			if(ph_dbg_draw_mask.test(phDBgDrawPositiveTries))
				DBG_DrawTri(T,V_array,D3DCOLOR_XRGB(255,0,0));
#endif	
//				if(ret>10) continue;
				if(!b_pushing&&(!intersect||no_last_pos))
					ret+=T::Collide(
					vertices[0],
					vertices[1],
					vertices[2],
					&tri,
					o1,
					o2,
					3,
					CONTACT(contact, ret * skip),   skip);
				if(no_last_pos)
					pos_tries.push_back(tri);
		}
	}

	//if(intersect) ret=0;
	xr_vector<Triangle>::iterator i;


	if(neg_depth<dInfinity&&intersect)
	{
		bool include = true;
		if(no_last_pos)
			for(i=pos_tries.begin();pos_tries.end() != i;++i)
			{	
				VERIFY(neg_tri.T);
				if(TriContainPoint(
					(dReal*)&V_array[i->T->verts[0]],
					(dReal*)&V_array[i->T->verts[1]],
					(dReal*)&V_array[i->T->verts[2]],
					i->norm,i->side0,
					i->side1,p))
					if(
						!((dDOT(neg_tri.norm,(dReal*)&V_array[i->T->verts[0]])-neg_tri.pos)>0.f)||
						!((dDOT(neg_tri.norm,(dReal*)&V_array[i->T->verts[1]])-neg_tri.pos)>0.f)||
						!((dDOT(neg_tri.norm,(dReal*)&V_array[i->T->verts[2]])-neg_tri.pos)>0.f)
						){
							include=false;
							break;
						}
			};

		if(include){
			VERIFY(neg_tri.T&&neg_tri.dist!=-dInfinity);
			ret+=T::CollidePlain(
				neg_tri.side0,neg_tri.side1,neg_tri.norm,
				neg_tri.T,
				neg_tri.dist,
				o1,o2,flags,
				CONTACT(contact, ret * skip),
				skip);	
				*pushing_neg=!!ret;
		}

	}


	//for(i=pos_tries.begin();pos_tries.end() != i;++i){
	//	CDB::TRI* T=i->T;
	//	ret+=dTriPrimitive(
	//		(const dReal*)&V_array[T->verts[0]],
	//		(const dReal*)&V_array[T->verts[1]],
	//		(const dReal*)&V_array[T->verts[2]],
	//		T,
	//		o1,
	//		o2,
	//		3,
	//		CONTACT(contact, ret * skip),   skip);

	//}



	if(b_neg_depth<dInfinity&&intersect){

		bool include = true;
		if(no_last_pos)
			for(i=pos_tries.begin();pos_tries.end() != i;++i){
				VERIFY(b_neg_tri.T&&b_neg_tri.dist!=-dInfinity);
				if(
					!((dDOT(b_neg_tri.norm,(dReal*)&V_array[i->T->verts[0]])-b_neg_tri.pos)>0.f)||
					!((dDOT(b_neg_tri.norm,(dReal*)&V_array[i->T->verts[1]])-b_neg_tri.pos)>0.f)||
					!((dDOT(b_neg_tri.norm,(dReal*)&V_array[i->T->verts[2]])-b_neg_tri.pos)>0.f)

					){
						include=false;
						break;
					}
			};

		if(include)	
		{	
			VERIFY(b_neg_tri.T);
			ret+=T::CollidePlain(
				b_neg_tri.side0,
				b_neg_tri.side1,
				b_neg_tri.norm,
				b_neg_tri.T,
				b_neg_tri.dist,
				o1,o2,flags,
				CONTACT(contact, ret * skip),
				skip);	
			*pushing_b_neg=!!ret;
		}

	}
	dVectorSet(last_pos,p);
	return ret;
}
#endif