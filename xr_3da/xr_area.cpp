#include "stdafx.h"
#include "xr_area.h"
#include "xr_object.h"
#include "xrLevel.h"
#include "xr_creator.h"
#include "feel_sound.h"
#include "x_ray.h"
#include "GameFont.h"

using namespace	Collide;

IC void minmax(float &mn, float &mx) { if (mn > mx) swap(mn,mx); }

void __stdcall _sound_event	(sound* S, float range)
{
	if (S&&S->feedback){
		const CSound_params*	p	= S->feedback->get_params();
		VERIFY					(p);

		// Query objects
		pCreator->ObjectSpace.GetNearest	(p->position,range);

		// Iterate
		CObjectSpace::NL_IT		it	= pCreator->ObjectSpace.q_nearest.begin	();
		CObjectSpace::NL_IT		end	= pCreator->ObjectSpace.q_nearest.end	();
		for (; it!=end; it++)
		{
			CObject*	O		= *it;
			Feel::Sound* L		= dynamic_cast<Feel::Sound*>(O);
			if (0==L)			continue;

			// Energy and signal
			Fvector				Oc;
			O->clCenter			(Oc);
			float D				= p->position.distance_to(Oc);
			float A				= p->min_distance/(psSoundRolloff*D);					// (Dmin*V)/(R*D) 
			clamp				(A,0.f,1.f);
			float Power			= A*p->volume;
			if (Power>EPS_S)	L->feel_sound_new	(S->g_object,S->g_type,p->position,Power);
		}
	}
}

//----------------------------------------------------------------------
// Class	: CObjectSpace
// Purpose	: stores space slots
//----------------------------------------------------------------------
CObjectSpace::CObjectSpace( )
{
	dwQueryID	= 0;
	sh_debug	= 0;
	
	sh_debug	= Device.Shader.Create	("debug\\wireframe","$null");
}
//----------------------------------------------------------------------
CObjectSpace::~CObjectSpace( )
{
	Sound->set_geometry_occ		(NULL);
	Sound->set_handler			(NULL);

	Device.Shader.Delete		(sh_debug);
}

//----------------------------------------------------------------------
IC void CObjectSpace::Object_Register		( CObject *O )
{
	R_ASSERT			(O);
	
	Irect				rect;
	CCFModel*			M = O->CFORM();
	R_ASSERT			(M);

	GetRect				(M, rect);
	M->rect_last		= rect;
	
	// add to slots
	for (int ix=rect.x1; ix<=rect.x2; ix++)	
		for (int iz=rect.y1; iz<=rect.y2; iz++)
			Dynamic(ix,iz).lst.push_back(O);
}
//----------------------------------------------------------------------
IC void CObjectSpace::Object_Move			( CObject *O ) 
{
	VERIFY		(O);
	CCFModel*	M = O->CFORM();

	Irect&		r0	= M->rect_last;
	Irect		r1;
	GetRect		(M, r1);
	if (r0.cmp(r1))	return;

	int 		ix, iz;
	for (ix=r0.x1; ix<=r0.x2; ix++)		// remove from slots
		for (iz=r0.y1; iz<=r0.y2; iz++)
			if (!r1.in(ix,iz))	{
				vector<CObject*>&	lst = Dynamic(ix,iz).lst;
				lst.erase(remove(lst.begin(),lst.end(),O),lst.end());
			}
			
			for (ix=r1.x1;ix<=r1.x2;ix++)		// add to slots
				for (iz=r1.y1;iz<=r1.y2;iz++)
					if (!r0.in(ix,iz))	Dynamic(ix,iz).lst.push_back(O);
					
					M->rect_last.set( r1 );				// set model last rect
}
//----------------------------------------------------------------------
IC void CObjectSpace::Object_Unregister		( CObject *O )
{
	R_ASSERT	(O);
	CCFModel*	M = O->CFORM();
	if (M){
		Irect&	r0 = M->rect_last;

		int 	ix, iz;
		for (ix=r0.x1; ix<=r0.x2; ix++){
			for (iz=r0.y1; iz<=r0.y2; iz++){
				vector<CObject*>&	lst = Dynamic(ix,iz).lst;
				lst.erase(remove(lst.begin(),lst.end(),O),lst.end());
			}
		}
		M->rect_last.null();
	}
}
//----------------------------------------------------------------------
IC int	CObjectSpace::GetNearest ( const Fvector &point, float range )
{
	dwQueryID			++;
	q_nearest.clear		( );
	Irect				rect;

	GetRect				( point, rect, range );
	Fsphere				Q; Q.set(point,range);

	CObject*			t_object;
	CCFModel*			t_model;
	int 				ix, iz;
	for (ix=rect.x1;ix<=rect.x2;ix++)
		for (iz=rect.y1;iz<=rect.y2;iz++)
			for (u32 q=0; q<Dynamic(ix, iz).lst.size(); q++){
				t_object	= Dynamic(ix, iz).lst[q];
				t_model		= t_object->CFORM();
				if (!t_model->enabled)					continue;
				if (t_model->dwQueryID!=dwQueryID) {
					t_model->dwQueryID=dwQueryID;
					Fsphere	mS;
					t_object->clXFORM().transform_tiny	(mS.P,t_model->getSphere().P);
					mS.R								= t_model->getRadius();
					if (Q.intersect(mS)) 
						q_nearest.push_back(t_object);
				}
			}
	return q_nearest.size();
}
//----------------------------------------------------------------------
IC int   CObjectSpace::GetNearest( CCFModel* obj, float range ){
	obj->Enable 		( false ); // self exclude from test
	Fvector				P;
	obj->Owner()->clXFORM().transform_tiny(P,obj->getSphere().P);
	int res				= GetNearest( P, range + obj->getRadius() );
	obj->EnableRollback	( );
	return				res;
}
//----------------------------------------------------------------------
IC void CObjectSpace::GetRect	( const Fvector &center, Irect &rect, float range ){
	rect.x1	= TransX(center.x - range);
	rect.y1	= TransZ(center.z - range);
	rect.x2	= TransX(center.x + range);
	rect.y2	= TransZ(center.z + range);
}
//----------------------------------------------------------------------
IC void CObjectSpace::GetRect	( const CCFModel *obj, Irect &rect ){
	VERIFY				( obj );
	VERIFY				( obj->owner );
	Fbox				bb;
	bb.xform			(obj->bv_box,obj->Owner()->svXFORM());
	rect.x1				= TransX(bb.min.x);
	rect.y1				= TransZ(bb.min.z);
	rect.x2				= TransX(bb.max.x);
	rect.y2				= TransZ(bb.max.z);
}
//----------------------------------------------------------------------
void CObjectSpace::Load	(IReader *F)
{
	R_ASSERT			(F);
	int					x_count, z_count;

	hdrCFORM			H;
	F->r				(&H,sizeof(hdrCFORM));
	Fvector*	verts	= (Fvector*)F->pointer();
	CDB::TRI*	tris	= (CDB::TRI*)(verts+H.vertcount);
	if (2 == H.version)
	{
		// fill materials-guids (.dummy) with u32(-1)
		for (u32 it=0; it<H.facecount; it++)
		{
			CDB::TRI*	T	= tris+it;
			T->dummy		= u32(-1);
		}
	} else {
		R_ASSERT			(CFORM_CURRENT_VERSION==H.version);
	}
	pCreator->Load_GameSpecific_CFORM	( tris, H.facecount );
	Static.build						( verts, H.vertcount, tris, H.facecount );
    Msg						("* Level CFORM memory usage: %dK",Static.memory()/1024);

	// CForm
	x_count					= iCeil((H.aabb.max.x-H.aabb.min.x)/CL_SLOT_SIZE);
	z_count					= iCeil((H.aabb.max.z-H.aabb.min.z)/CL_SLOT_SIZE);
	Static_Shift.invert		(H.aabb.min);
	Dynamic.SetSize			( x_count, z_count );

	// Sound
	Sound->set_geometry_occ		( &Static );
	Sound->set_handler			( _sound_event );
}
//----------------------------------------------------------------------
void CObjectSpace::dbgRender()
{
	R_ASSERT(bDebug);

	RCache.set_Shader(sh_debug);
	for (u32 i=0; i<q_debug.boxes.size(); i++)
	{
		Fobb&		obb		= q_debug.boxes[i];
		Fmatrix		X,S,R;
		obb.xform_get(X);
		RCache.dbg_DrawOBB(X,obb.m_halfsize,D3DCOLOR_XRGB(255,0,0));
		S.scale		(obb.m_halfsize);
		R.mul		(X,S);
		RCache.dbg_DrawEllipse(R,D3DCOLOR_XRGB(0,0,255));
	}
	q_debug.boxes.clear();

	for (i=0; i<dbg_E.size(); i++)
	{
		RCache.dbg_DrawEllipse(dbg_E[i],D3DCOLOR_XRGB(0,255,0));
	}
	dbg_E.clear();

	for (i=0; i<dbg_S.size(); i++)
	{
		pair<Fsphere,u32>& P = dbg_S[i];
		Fsphere&	S = P.first;
		Fmatrix		M;
		M.scale		(S.R,S.R,S.R);
		M.translate_over(S.P);
		RCache.dbg_DrawEllipse(M,P.second);
	}
	dbg_S.clear();

	// render slots
	Fvector c;
	u32	C1		= D3DCOLOR_XRGB(0,64,0);
	u32	C1s		= D3DCOLOR_XRGB(0,255,0);
	u32	C2		= D3DCOLOR_XRGB(100,100,100);
	u32	C2s		= D3DCOLOR_XRGB(170,170,170);

	float cl = CL_SLOT_SIZE;
	float cl_2 = CL_SLOT_SIZE/2-.05f;
	for (int x=0; x<Dynamic.x_count; x++){
		for (int z=0; z<Dynamic.z_count; z++){
			c.set(-Static_Shift.x+cl*x+cl_2,0.1f,-Static_Shift.z+cl*z+cl_2);
			BOOL B=FALSE;
			for (i=0; i<dbg_Slot.size(); i++){
				B=fsimilar(dbg_Slot[i].x,float(x))&&(fsimilar(dbg_Slot[i].y,float(z)));
				if (B) break;
			}
			
			RCache.dbg_DrawAABB(c,1.f,.1f,1.f,B?C1s:C1);
			RCache.dbg_DrawAABB(c,cl_2,0,cl_2,B?C2s:C2);
		}
	}
	c.set(pCreator->CurrentEntity()->Position());
}
