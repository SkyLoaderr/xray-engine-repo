#include "stdafx.h"
#include "xr_area.h"
#include "xr_object.h"
#include "xrLevel.h"


#include "xr_creator.h"

#include "x_ray.h"
#include "xr_smallfont.h"

using namespace	Collide;

//----------------------------------------------------------------------
// Class	: CObjectSpace
// Purpose	: stores space slots
//----------------------------------------------------------------------
CObjectSpace::CObjectSpace( ){
	dwQueryID = 0x0;
}
//----------------------------------------------------------------------

CObjectSpace::~CObjectSpace( ){
}
//----------------------------------------------------------------------
IC void CObjectSpace::Object_Register		( CObject *O )
{
	R_ASSERT			(O);
	
	Irect				rect;
	CCFModel*			M = O->CFORM();
	R_ASSERT			(M);

	GetRect				(M, rect);
	M->last_rect		= rect;
	
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

	Irect&		r0	= M->last_rect;
	Irect		r1;
	GetRect		(M, r1);
	if (r0.cmp(r1))	return;

	int 		ix, iz;
	for (ix=r0.x1; ix<=r0.x2; ix++)		// remove from slots
		for (iz=r0.y1; iz<=r0.y2; iz++)
			if (!r1.in(ix,iz))	Dynamic(ix,iz).lst.erase(O);

	for (ix=r1.x1;ix<=r1.x2;ix++)		// add to slots
		for (iz=r1.y1;iz<=r1.y2;iz++)
			if (!r0.in(ix,iz))	Dynamic(ix,iz).lst.push_back(O);

	M->last_rect.set( r1 );				// set model last rect
}
//----------------------------------------------------------------------
IC void CObjectSpace::Object_Unregister		( CObject *O )
{
	R_ASSERT	(O);
	CCFModel*	M = O->CFORM();

	Irect&		r0 = M->last_rect;

	int 		ix, iz;
	for (ix=r0.x1; ix<=r0.x2; ix++)
		for (iz=r0.y1; iz<=r0.y2; iz++)
			Dynamic(ix,iz).lst.erase( O );

	M->last_rect.invalidate();
}
//----------------------------------------------------------------------
IC int	CObjectSpace::GetNearest ( const Fvector &point, float range )
{
	nearest_list.clear	( );
	Irect				rect;

	GetRect				( point, rect, range );
	Fsphere				Q; Q.set(point,range);

	CCFModel*			target;
	int 				ix, iz;
	for (ix=rect.x1;ix<=rect.x2;ix++)
		for (iz=rect.y1;iz<=rect.y2;iz++)
			for (DWORD q=0; q<Dynamic(ix, iz).lst.size(); q++){
				target	= Dynamic(ix, iz).lst[q]->CFORM();
				if (!target->enabled)					continue;
				if (target->dwQueryID!=dwQueryID) {
					target->dwQueryID=dwQueryID;
					if (Q.intersect(target->Sphere)) 
						nearest_list.push_back(target);
				}
			}
	return nearest_list.size();
}
//----------------------------------------------------------------------
IC int   CObjectSpace::GetNearest( CCFModel* obj, float range ){
	obj->Enable 		( false ); // self exclude from test
	int res				= GetNearest( obj->Sphere.P, range + obj->Sphere.R );
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
	Fvector				min, max;
	min.set				(obj->s_box.min);
	max.set				(obj->s_box.max);
	obj->owner->svTransform.transform(min);
	obj->owner->svTransform.transform(max);
	minmax				(min.x, max.x);
	minmax				(min.z, max.z);
	rect.x1				= TransX(min.x);
	rect.y1				= TransZ(min.z);
	rect.x2				= TransX(max.x);
	rect.y2				= TransZ(max.z);
}
//----------------------------------------------------------------------
BOOL CObjectSpace::TestNearestObject(CCFModel *object, const Fvector& center, float range){
	dwQueryID++;

	object->Enable 		( false ); // self exclude from test
	GetNearest			( center, range );
	object->EnableRollback( );

	CCFModel**			_it	 = nearest_list.begin	();
	CCFModel**			_end = nearest_list.end		();
	for ( ; _it!=_end; _it++ )
	{
		(*_it)->Owner()->OnNear(object->Owner());
		/*
		if (!object->owner->OnNear(target)) {
			nearest_list.erase(_it-nearest_list.begin());
			_it--;
			_end = nearest_list.end();
		}
		*/
	}
	return nearest_list.size();
}
//----------------------------------------------------------------------

void CObjectSpace::Load( CStream *F ){
	R_ASSERT			(F);
	int					x_count, z_count;

	hdrCFORM			H;
	F->Read				(&H,sizeof(hdrCFORM));
	R_ASSERT			(CFORM_CURRENT_VERSION==H.version);

	Fvector*	verts	= (Fvector*)F->Pointer();
	RAPID::tri*	tris	= (RAPID::tri*)(verts+H.vertcount);
	Static.BuildModel	( verts, H.vertcount, tris, H.facecount );
	Msg("* Level CFORM memory usage: %dK",Static.MemoryUsage()/1024);

	// CForm
	x_count				= iCeil((H.aabb.max.x-H.aabb.min.x)/CL_SLOT_SIZE);
	z_count				= iCeil((H.aabb.max.z-H.aabb.min.z)/CL_SLOT_SIZE);
	Static_Shift.invert	(H.aabb.min);
	Dynamic.SetSize		( x_count, z_count );
}
//----------------------------------------------------------------------
void CObjectSpace::dbgRender()
{
	R_ASSERT(bDebug);

	pApp->pFont->Out(0,0,"Box count: %d",q_debug.boxes.size());
	for (DWORD i=0; i<q_debug.boxes.size(); i++)
	{
		Fobb&		obb		= q_debug.boxes[i];
		Fmatrix		X,S,R;
		obb.xform_get(X);
		Device.Primitive.dbg_DrawOBB(X,obb.m_halfsize,D3DCOLOR_XRGB(255,0,0));
		S.scale		(obb.m_halfsize);
		R.mul		(X,S);
		Device.Primitive.dbg_DrawEllipse(R,D3DCOLOR_XRGB(0,0,255));
	}
	q_debug.boxes.clear();

	for (i=0; i<dbg_E.size(); i++)
	{
		Device.Primitive.dbg_DrawEllipse(dbg_E[i],D3DCOLOR_XRGB(0,255,0));
	}
	dbg_E.clear();

	for (i=0; i<dbg_S.size(); i++)
	{
		pair<Fsphere,DWORD>& P = dbg_S[i];
		Fsphere&	S = P.first;
		Fmatrix		M;
		M.scale		(S.R,S.R,S.R);
		M.translate_over(S.P);
		Device.Primitive.dbg_DrawEllipse(M,P.second);
	}
	dbg_S.clear();

	// render slots
	Fvector c;
	DWORD	C1		= D3DCOLOR_XRGB(0,64,0);
	DWORD	C1s		= D3DCOLOR_XRGB(0,255,0);
	DWORD	C2		= D3DCOLOR_XRGB(100,100,100);
	DWORD	C2s		= D3DCOLOR_XRGB(170,170,170);

	float cl = CL_SLOT_SIZE;
	float of = .1f;
	float cl_2 = CL_SLOT_SIZE/2-.05f;
	for (int x=0; x<Dynamic.x_count; x++){
		for (int z=0; z<Dynamic.z_count; z++){
			c.set(-Static_Shift.x+cl*x+cl_2,0.1f,-Static_Shift.z+cl*z+cl_2);
			BOOL B=FALSE;
			for (i=0; i<dbg_Slot.size(); i++){
				B=fsimilar(dbg_Slot[i].x,float(x))&&(fsimilar(dbg_Slot[i].y,float(z)));
				if (B) break;
			}
			
			Device.Primitive.dbg_DrawAABB(c,1.f,.1f,1.f,B?C1s:C1);
			Device.Primitive.dbg_DrawAABB(c,cl_2,0,cl_2,B?C2s:C2);
		}
	}
	c.set(pCreator->CurrentEntity()->Position());
	pApp->pFont->Out(-1.f,0.5f,"Slot {%d,%d}",TransX(c.x),TransZ(c.z));
}
