#include "stdafx.h"
#pragma hdrstop

#include "soundrender_emitter.h"
#include "soundrender_core.h"

extern	u32				psSoundModel;
extern	u32				psSoundFreq;
extern	float			psSoundVEffects;

//////////////////////////////////////////////////////////////////////
CSoundRender_Emitter::CSoundRender_Emitter(void)
{

#ifdef DEBUG
	static	u32			incrementalID = 0;
	dbg_ID				= ++incrementalID;
#endif
	target				= NULL;
	source				= NULL;
	owner				= NULL;
	smooth_volume		= 1.f;
	occluder_volume		= 1.f;
	occluder[0].set		(0,0,0);
	occluder[1].set		(0,0,0);
	occluder[2].set		(0,0,0);
	state				= stStopped;
	position			= 0;
	bMoved				= TRUE;
	b2D					= FALSE;
	dwTimeStarted		= 0;
	dwTimeToStop		= 0;
	dwTimeToPropagade	= 0;
	marker				= 0xabababab;
	starting_delay		= 0.f;
	priority_scale		= 1.f;
}

CSoundRender_Emitter::~CSoundRender_Emitter(void)
{
	// try to release dependencies, events, for example
	Event_ReleaseOwner	();
}

//////////////////////////////////////////////////////////////////////
void CSoundRender_Emitter::Event_ReleaseOwner()
{
	if	(0==owner)		return;

	for (u32 it=0; it<SoundRender->s_events.size(); it++)
	{
		if (owner == SoundRender->s_events[it].first)	
		{
			SoundRender->s_events.erase(SoundRender->s_events.begin()+it);
			it	--;
		}
	}
}
void CSoundRender_Emitter::Event_Propagade	()
{
	dwTimeToPropagade			+= ::Random.randI	(sdef_event_pulse-30,sdef_event_pulse+30);
	if (0==owner)					return;
	if (0==owner->g_type)			return;
	if (0==SoundRender->Handler)	return;

	// Calculate range
	float	limitV					= .01f;
	float	clip					= (p_source.min_distance*p_source.volume) / (psSoundRolloff*limitV); // (Dmin*V)/(R*V')
	float	range					= _min(p_source.max_distance,clip);
	if	(clip<0)					return;

	// Inform objects
	SoundRender->s_events.push_back	(mk_pair(owner,range));
}
