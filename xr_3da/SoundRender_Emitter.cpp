#include "stdafx.h"
#include "soundrender_emitter.h"
#include "soundrender_core.h"

extern	u32				psSoundModel;
extern	u32				psSoundFreq;
extern	float			psSoundVEffects;

//////////////////////////////////////////////////////////////////////
CSoundRender_Emitter::CSoundRender_Emitter(void)
{
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
	dwTimeStarted		= 0;
	dwTimeToStop		= 0;
	dwTimeToPropagade	= 0;
}

CSoundRender_Emitter::~CSoundRender_Emitter(void)
{
}
//////////////////////////////////////////////////////////////////////
void CSoundRender_Emitter::PropagadeEvent()
{
	dwTimeToPropagade			+= ::Random.randI	(sdef_event_pulse-30,sdef_event_pulse+30);
	if (0==owner)					return;
	if (0==owner->g_type)			return;
	if (0==SoundRender.Handler)		return;

	// Calculate range
	float	limitV					= .01f;
	float	clip					= (p_source.min_distance*p_source.volume) / (psSoundRolloff*limitV); // (Dmin*V)/(R*V')
	float	range					= _min(p_source.max_distance,clip);
	if	(clip<0)					return;

	// Inform objects
	SoundRender.Handler				(owner,range);
}
