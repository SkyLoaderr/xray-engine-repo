#include "stdafx.h"
#pragma hdrstop

#include "cl_intersect.h"
#include "SoundRender_Core.h"
#include "SoundRender_Emitter.h"
#include "SoundRender_Target.h"
#include "SoundRender_Source.h"

CSoundRender_Emitter*	CSoundRender_Core::i_play(ref_sound* S, BOOL _loop, float delay)
{
	CSoundRender_Emitter* E	=	xr_new<CSoundRender_Emitter>();
	S->feedback				=	E;
	E->start				(S,_loop,delay);
	s_emitters.push_back	(E);
	return E;
}

void CSoundRender_Core::update	( const Fvector& P, const Fvector& D, const Fvector& N, float dt )
{
	u32 it;

	if (0==bReady)				return;

	s_emitters_u	++;

	// Firstly update emitters, which are now being rendered
	//Msg	("! update: r-emitters");
	for (it=0; it<s_targets.size(); it++)
	{
		CSoundRender_Target*	T	= s_targets	[it];
		CSoundRender_Emitter*	E	= T->get_emitter();
		if (E) {
			E->update	(dt);
			E->marker	= s_emitters_u;
			E			= T->get_emitter();	// update can stop itself
			if (E)		T->priority	= E->priority();
			else		T->priority	= -1;
		} else {
			T->priority	= -1;
		}
	}

	// Update emmitters
	//Msg	("! update: emitters");
	for (it=0; it<s_emitters.size(); it++)
	{
		CSoundRender_Emitter*	pEmitter = s_emitters[it];
		if (pEmitter->marker!=s_emitters_u)
		{
			pEmitter->update		(dt);
			pEmitter->marker		= s_emitters_u;
		}
		if (!pEmitter->isPlaying())		
		{
			// Stopped
			xr_delete		(pEmitter);
			s_emitters.erase(s_emitters.begin()+it);
			it--;
		}
	}

	// Get currently rendering emitters
	//Msg	("! update: targets");
	s_targets_defer.clear	();
	s_targets_pu			++;
	u32 PU					= s_targets_pu%s_targets.size();
	for (it=0; it<s_targets.size(); it++)
	{
		CSoundRender_Target*	T	= s_targets	[it];
		if (T->get_emitter())
		{
			// Has emmitter, maybe just not started rendering
			if		(T->get_Rendering())	
			{
				/*if	(PU == it)*/	T->fill_parameters	();
				T->update		();
			}
			else 	
				s_targets_defer.push_back		(T);
		}
	}

	// Commit parameters from pending targets
	if (!s_targets_defer.empty())
	{
		//Msg	("! update: start render - commit");
		s_targets_defer.erase	(std::unique(s_targets_defer.begin(),s_targets_defer.end()),s_targets_defer.end());
		for (it=0; it<s_targets_defer.size(); it++)
			s_targets_defer[it]->fill_parameters();
	}

	// update EAX
    if (psSoundFlags.test(ssEAX)&&bEAX){
        if (bListenerMoved){
            bListenerMoved			= FALSE;
            e_target				= *get_environment	(P);
        }
        e_current.lerp				(e_current,e_target,dt);

        i_eax_listener_set			(&e_current);
		i_eax_commit_setting		();
	}

    // update listener
    update_listener					(P,D,N,dt);
    
	// Start rendering of pending targets
	if (!s_targets_defer.empty())
	{
		//Msg	("! update: start render");
		for (it=0; it<s_targets_defer.size(); it++)
			s_targets_defer[it]->render	();
	}
}

void	CSoundRender_Core::update_events		()
{
	for (u32 it=0; it<s_events.size(); it++)
	{
		event&	E	= s_events[it];
		Handler		(E.first,E.second);
	}
	s_events.clear();
}

void	CSoundRender_Core::statistic			(CSound_stats&  dest)
{
	dest._rendered		= 0;
	for (u32 it=0; it<s_targets.size(); it++)	{
		CSoundRender_Target*	T	= s_targets	[it];
		if (T->get_emitter() && T->get_Rendering())	dest._rendered++;
	}
	dest._simulated		= s_emitters.size();
	dest._cache_hits	= cache._stat_hit;
	dest._cache_misses	= cache._stat_miss;
	cache.stats_clear	();
}

BOOL	CSoundRender_Core::get_occlusion(Fvector& P, float R, Fvector* occ)
{
	if (0==geom_MODEL)		return FALSE;

	// Calculate RAY params
	Fvector base			= listener_position();
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
		const CDB::RESULT*	R = geom_DB.r_begin			();
		const CDB::TRI&		T = geom_MODEL->get_tris	() [ R->id ];
		const Fvector*		V = geom_MODEL->get_verts	();
		occ[0].set			(V[T.verts[0]]);
		occ[1].set			(V[T.verts[1]]);
		occ[2].set			(V[T.verts[2]]);
		return TRUE;
	}
}













