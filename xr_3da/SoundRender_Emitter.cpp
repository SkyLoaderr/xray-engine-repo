#include "stdafx.h"
#include "soundrender_emitter.h"
#include "xr_creator.h"
#include "xr_object.h"
#include "feel_sound.h"

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
	dwTimeToPropagade		+= ::Random.randI	(sdef_event_pulse-30,sdef_event_pulse+30);
	if (0==owner)			return;
	if (0==owner->g_type)	return;
	if (0==pCreator)		return;

	// Calculate range
	float	limitV			= .01f;
	float	clip			= (p_source.min_distance*p_source.volume) / (psSoundRolloff*limitV); // (Dmin*V)/(R*V')
	float	range			= _min(p_source.max_distance,clip);
	if	(clip<0)			return;

	// Query objects
	pCreator->ObjectSpace.GetNearest	(p_source.position,range);

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
		float D				= p_source.position.distance_to(Oc);
		float A				= p_source.min_distance/(psSoundRolloff*D);					// (Dmin*V)/(R*D) 
		clamp				(A,0.f,1.f);
		float Power			= A*p_source.volume;
		if (Power>EPS_S)	L->feel_sound_new	(owner->g_object,owner->g_type,p_source.position,Power);
	}
}
