#include "stdafx.h"
#include "igame_level.h"

#include "xr_area.h"
#include "xr_object.h"
#include "xrLevel.h"
#include "feel_sound.h"
#include "x_ray.h"
#include "GameFont.h"

using namespace	Collide;

IC void minmax(float &mn, float &mx) { if (mn > mx) std::swap(mn,mx); }

void __stdcall _sound_event	(sound* S, float range)
{
#pragma todo("Check why sometimes 'S->feedback==0'")
	if ( S&&S->feedback )
	{
		const CSound_params*	p	= S->feedback->get_params();
		VERIFY					(p);

		// Query objects
		Fvector					bb_size	= {range,range,range};
		g_SpatialSpace.q_box	(0,STYPE_REACTTOSOUND,p->position,bb_size);

		// Iterate
		xr_vector<ISpatial*>::iterator	it	= g_SpatialSpace.q_result.begin	();
		xr_vector<ISpatial*>::iterator	end	= g_SpatialSpace.q_result.end	();
		for (; it!=end; it++)
		{
			Feel::Sound* L		= dynamic_cast<Feel::Sound*>(*it);
			if (0==L)			continue;

			// Energy and signal
			float D				= p->position.distance_to((*it)->spatial.center);
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
CObjectSpace::CObjectSpace	( )
{
	sh_debug					= 0;
	sh_debug					= Device.Shader.Create	("debug\\wireframe","$null");
}
//----------------------------------------------------------------------
CObjectSpace::~CObjectSpace	( )
{
	Sound->set_geometry_occ		(NULL);
	Sound->set_handler			(NULL);

	Device.Shader.Delete		(sh_debug);
}
//----------------------------------------------------------------------
IC int	CObjectSpace::GetNearest ( const Fvector &point, float range )
{
	// Query objects
	q_nearest.clear		( );
	Fsphere				Q;	Q.set(point,range);
	Fvector				B;	B.set(range,range,range);
	g_SpatialSpace.q_box(0,STYPE_COLLIDEABLE,point,B);

	// Iterate
	xr_vector<ISpatial*>::iterator	it	= g_SpatialSpace.q_result.begin	();
	xr_vector<ISpatial*>::iterator	end	= g_SpatialSpace.q_result.end	();
	for (; it!=end; it++)
	{
		CObject* O				= dynamic_cast<CObject*>(*it);
		if (0==O)				continue;
		Fsphere mS				= { O->spatial.center, O->spatial.radius	};
		if (Q.intersect(mS))	q_nearest.push_back(O);
	}

	return q_nearest.size();
}
//----------------------------------------------------------------------
IC int   CObjectSpace::GetNearest( ICollisionForm* obj, float range ){
	obj->Enable 		( false ); // self exclude from test
	Fvector				P;
	obj->Owner()->XFORM().transform_tiny(P,obj->getSphere().P);
	int res				= GetNearest( P, range + obj->getRadius() );
	obj->EnableRollback	( );
	return				res;
}
//----------------------------------------------------------------------
static void __stdcall	build_callback	(Fvector* V, int Vcnt, CDB::TRI* T, int Tcnt, void* params)
{
	g_pGameLevel->Load_GameSpecific_CFORM( T, Tcnt );
}
void CObjectSpace::Load	()
{
	IReader *F					= FS.r_open	("$level$", "level.cform");
	R_ASSERT					(F);

	hdrCFORM					H;
	F->r						(&H,sizeof(hdrCFORM));
	Fvector*	verts			= (Fvector*)F->pointer();
	CDB::TRI*	tris			= (CDB::TRI*)(verts+H.vertcount);
	R_ASSERT					(CFORM_CURRENT_VERSION==H.version);
	Static.build				( verts, H.vertcount, tris, H.facecount, build_callback );

	g_SpatialSpace.initialize	(H.aabb);
	Sound->set_geometry_occ		( &Static );
	Sound->set_handler			( _sound_event );

	FS.r_close					(F);
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

	for (i=0; i<dbg_S.size(); i++)
	{
		std::pair<Fsphere,u32>& P = dbg_S[i];
		Fsphere&	S = P.first;
		Fmatrix		M;
		M.scale		(S.R,S.R,S.R);
		M.translate_over(S.P);
		RCache.dbg_DrawEllipse(M,P.second);
	}
	dbg_S.clear();
}
