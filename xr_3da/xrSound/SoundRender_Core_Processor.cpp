#include "stdafx.h"
#pragma hdrstop

#include "cl_intersect.h"
#include "SoundRender_Core.h"
#include "SoundRender_Emitter.h"
#include "SoundRender_Target.h"

CSoundRender_Emitter*	CSoundRender_Core::i_play(sound* S, BOOL _loop )
{
	CSoundRender_Emitter* E	=	xr_new<CSoundRender_Emitter>();
	S->feedback				=	E;
	E->start				(S,_loop);
	s_emitters.push_back	(E);
	return E;
}

void CSoundRender_Core::update	( const Fvector& P, const Fvector& D, const Fvector& N, float dt )
{
	u32 it;

	// Update emmitters
	for (it=0; it<s_emitters.size(); it++)
	{
		CSoundRender_Emitter*	pEmitter = s_emitters[it];
		pEmitter->update		(dt);
		if (!pEmitter->isPlaying())		
		{
			// Stopped
			xr_delete		(pEmitter);
			s_emitters.erase(s_emitters.begin()+it);
			it--;
		}
	}

	// Get currently rendering emitters
	s_targets_defer.clear	();
	for (it=0; it<s_targets.size(); it++)
	{
		CSoundRender_Target*	T	= s_targets	[it];
		if (T->get_emitter())
		{
			// Has emmitter, maybe just not started rendering
			if		(T->get_Rendering())	
				T->update	();
			else 	
				s_targets_defer.push_back		(T);
		}
	}

	// Update listener
	if (pListener)
	{
		clamp								(dt,EPS_S,1.f/10.f);
		Listener.vVelocity.sub				(P, Listener.vPosition );
		Listener.vVelocity.div				(dt);
		Listener.vPosition.set				(P);
		Listener.vOrientFront.set			(D);
		Listener.vOrientTop.set				(N);
		Listener.fDopplerFactor				= psSoundDoppler;
		Listener.fRolloffFactor				= psSoundRolloff;
		pListener->SetAllParameters			((DS3DLISTENER*)&Listener, DS3D_DEFERRED );
		pListener->CommitDeferredSettings	();
	}

	// Start rendering of pending targets
	for (it=0; it<s_targets_defer.size(); it++)
		s_targets_defer[it]->render	();
}

u32		CSoundRender_Core::stat_render	()
{
	u32 counter		= 0;
	for (it=0; it<s_targets.size(); it++)
	{
		CSoundRender_Target*	T	= s_targets	[it];
		if (T->get_emitter() && T->get_Rendering())	counter++;
	}
	return counter;
}

u32		CSoundRender_Core::stat_simulate()
{
	return s_emitters.size();
}

BOOL	CSoundRender_Core::get_occlusion(Fvector& P, float R, Fvector* occ)
{
	if (0==geom_MODEL)		return FALSE;

	// Calculate RAY params
	Fvector base			= Listener.vPosition;
	Fvector	pos,dir;
	float	range;
	pos.random_dir			();
	pos.mul					(R);
	pos.add					(P);
	dir.sub					(pos,base);
	range = dir.magnitude	();
	dir.div					(range);

	// 1. Check cached polygon
	float _u,_v,_range;
	if (CDB::TestRayTri(base,dir,occ,_u,_v,_range,true))
		if (_range>0 && _range<range) return TRUE;

	// 2. Polygon doesn't picked up - real database query
	geom_DB.ray_options			(CDB::OPT_ONLYNEAREST);
	geom_DB.ray_query			(geom_MODEL,base,dir,range);
	if (0==geom_DB.r_count()) {
		return FALSE;
	} else {
		// cache polygon
		const CDB::RESULT*	R = geom_DB.r_begin();
		const CDB::TRI&		T = geom_MODEL->get_tris() [ R->id ];
		occ[0].set	(*T.verts[0]);
		occ[1].set	(*T.verts[1]);
		occ[2].set	(*T.verts[2]);
		return TRUE;
	}
}
