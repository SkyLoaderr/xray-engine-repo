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

void	IGame_Level::SoundEvent	( ref_sound* S, float range )
{
	if (S->g_object&&S->g_object->getDestroy()) return;

	const CSound_params*	p	= S->feedback->get_params();
	VERIFY					(p);

	// Query objects
	Fvector					bb_size	= {range,range,range};
	g_SpatialSpace->q_box	(snd_ER,0,STYPE_REACTTOSOUND,p->position,bb_size);

	// Iterate
	xr_vector<ISpatial*>::iterator	it	= snd_ER.begin	();
	xr_vector<ISpatial*>::iterator	end	= snd_ER.end	();
	for (; it!=end; it++)
	{
		Feel::Sound* L		= (*it)->dcast_FeelSound	();
		if (0==L)			continue;

		// Energy and signal
		float D				= p->position.distance_to((*it)->spatial.center);
		float A				= p->min_distance/(psSoundRolloff*D);					// (Dmin*V)/(R*D) 
		clamp				(A,0.f,1.f);
		float Power			= A*p->volume;
		if (Power>EPS_S)	L->feel_sound_new	(S->g_object,S->g_type,p->position,Power);
	}
}

void __stdcall _sound_event	(ref_sound* S, float range)
{
	if ( g_pGameLevel && S && S->feedback )
		g_pGameLevel->SoundEvent(S,range);
}

//----------------------------------------------------------------------
// Class	: CObjectSpace
// Purpose	: stores space slots
//----------------------------------------------------------------------
CObjectSpace::CObjectSpace	( )
{
#ifdef DEBUG
	sh_debug.create				("debug\\wireframe","$null");
#endif
	m_BoundingVolume.invalidate	();
}
//----------------------------------------------------------------------
CObjectSpace::~CObjectSpace	( )
{
	Sound->set_geometry_occ		(NULL);
	Sound->set_handler			(NULL);
#ifdef DEBUG
	sh_debug.destroy			();
#endif
}
//----------------------------------------------------------------------
IC int	CObjectSpace::GetNearest ( const Fvector &point, float range )
{
	// Query objects
	q_nearest.clear		( );
	Fsphere				Q;	Q.set(point,range);
	Fvector				B;	B.set(range,range,range);
	g_SpatialSpace->q_box(r_spatial,0,STYPE_COLLIDEABLE,point,B);

	// Iterate
	xr_vector<ISpatial*>::iterator	it	= r_spatial.begin	();
	xr_vector<ISpatial*>::iterator	end	= r_spatial.end		();
	for (; it!=end; it++)		{
		CObject* O				= (*it)->dcast_CObject		();
		if (0==O)				continue;
		Fsphere mS				= { O->spatial.center, O->spatial.radius	};
		if (Q.intersect(mS))	q_nearest.push_back(O);
	}

	return q_nearest.size();
}
//----------------------------------------------------------------------
IC int   CObjectSpace::GetNearest( ICollisionForm* obj, float range )
{
	CObject*	O		= obj->Owner	();
	BOOL		E		= O->getEnabled	();
	O->setEnabled		(FALSE);
	int res				= GetNearest( O->spatial.center, range + O->spatial.radius );
	O->setEnabled		(E);
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

	m_BoundingVolume.set		(H.aabb);
	g_SpatialSpace->initialize	(H.aabb);
	Sound->set_geometry_occ		( &Static );
	Sound->set_handler			( _sound_event );

	FS.r_close					(F);
}
//----------------------------------------------------------------------
#ifdef DEBUG
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
#endif
