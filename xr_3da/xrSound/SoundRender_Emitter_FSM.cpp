#include "stdafx.h"
#pragma hdrstop

#include "SoundRender_Emitter.h"
#include "SoundRender_Core.h"
#include "SoundRender_Source.h"

XRSOUND_API extern float			psSoundCull				;

void CSoundRender_Emitter::update	(float dt)
{
	u32	dwTime			= SoundRender->Timer.GetElapsed_ms();

	switch (state)	
	{
	case stStopped:
		break;
	case stStartingDelayed:
	    starting_delay		-= dt;
    	if (starting_delay<=0) 
        	state			= stStarting;
    	break;
	case stStarting:
		dwTimeStarted		= dwTime;
		dwTimeToStop		= dwTime + source->dwTimeTotal;
		dwTimeToPropagade	= dwTime;
		fade_volume			= 1.f;
		occluder_volume		= SoundRender->get_occlusion	(p_source.position,.2f,occluder);
		smooth_volume		= p_source.volume*psSoundVEffects*occluder_volume;
		e_current = e_target= *SoundRender->get_environment	(p_source.position);
		if (update_culling(dt))	
		{
			state			=	stPlaying;
			position		=	0;
			SoundRender->i_start(this);
		}
		else state			=	stSimulating;
		break;
	case stStartingLoopedDelayed:
	    starting_delay		-= dt;
    	if (starting_delay<=0) 
	    	state			= stStartingLooped;
    	break;
	case stStartingLooped:
		dwTimeStarted		= dwTime;
		dwTimeToStop		= 0xffffffff;
		dwTimeToPropagade	= dwTime;
		fade_volume			= 1.f;
		occluder_volume		= SoundRender->get_occlusion	(p_source.position,.2f,occluder);
		smooth_volume		= p_source.volume*psSoundVEffects*occluder_volume;
		e_current = e_target= *SoundRender->get_environment	(p_source.position);
		if (update_culling(dt)){
			state		  	=	stPlayingLooped;
			position	  	=	0;
			SoundRender->i_start(this);
		}else state		  	=	stSimulatingLooped;
		break;
	case stPlaying:
		if (dwTime>=dwTimeToStop){
			// STOP
			state					=	stStopped;
			SoundRender->i_stop		(this);
		}else{
			if (!update_culling(dt)) {
				// switch to: SIMULATE
				state					=	stSimulating;		// switch state
				SoundRender->i_stop		(this);
			}else{
				// We are still playing
				update_environment	(dt);
			}
		}
		break;
	case stSimulating:
		if (dwTime>=dwTimeToStop){
			// STOP
			state					=	stStopped;
		}else{
			if (update_culling(dt)){
				// switch to: PLAY
				state					=	stPlaying;
				position				= 	(((dwTime-dwTimeStarted)%source->dwTimeTotal)*source->dwBytesPerMS);
//				position				= 	(((dwTime-dwTimeStarted)%source->dwTimeTotal)*source->dwBytesPerSec)/1000;
				SoundRender->i_start		(this);
			}
		}
		break;
	case stPlayingLooped:
		if (!update_culling(dt)){
			// switch to: SIMULATE
			state					=	stSimulatingLooped;	// switch state
			SoundRender->i_stop		(this);
		}else{
			// We are still playing
			update_environment	(dt);
		}
		break;
	case stSimulatingLooped:
		if (update_culling(dt)){
			// switch to: PLAY
			state					=	stPlayingLooped;	// switch state
			position				= (((dwTime-dwTimeStarted)%source->dwTimeTotal)*source->dwBytesPerMS);
//			position				= (((dwTime-dwTimeStarted)%source->dwTimeTotal)*source->dwBytesPerSec)/1000;
			SoundRender->i_start		(this);
		}
		break;
	}

	// footer
	bMoved				= FALSE;
	if (state != stStopped){
		if (dwTime	>=	dwTimeToPropagade)		Event_Propagade();
	} else if (owner)	{ owner->feedback = 0; owner	= 0; }
}

IC void	volume_lerp		(float& c, float t, float s, float dt)
{
	float diff		= t - c;
	float diff_a	= _abs(diff);
	if (diff_a<EPS_S) return;
	float mot		= s*dt;
	if (mot>diff_a) mot=diff_a;
	c				+= (diff/diff_a)*mot;
}

BOOL	CSoundRender_Emitter::update_culling	(float dt)
{
	// Check range
	float	dist		= SoundRender->listener_position().distance_to	(p_source.position);
	if (dist>p_source.max_distance)										{ smooth_volume = 0; return FALSE; }

	// Calc attenuated volume
	float att			= p_source.min_distance/(psSoundRolloff*dist);	clamp(att,0.f,1.f);
	float fade_scale	= (att*p_source.volume*psSoundVEffects<psSoundCull)?-1.f:1.f;
	fade_volume			+=	dt*1.f*fade_scale;
	clamp				(fade_volume,0.f,1.f);

	// Update occlusion
	volume_lerp			(occluder_volume,SoundRender->get_occlusion	(p_source.position,.2f,occluder),1.f,dt);
	clamp				(occluder_volume,0.f,1.f);

	// Update smoothing
	smooth_volume		= .9f*smooth_volume + .1f*(p_source.volume*psSoundVEffects*occluder_volume*fade_volume);

	if (smooth_volume<psSoundCull)							return FALSE;	// allow volume to go up

	// Here we has enought "PRIORITY" to be soundable
	// If we are playing already, return OK
	// --- else check availability of resources
	if (target)			return	TRUE;
	else				return	SoundRender->i_allow_play	(this);
}

float	CSoundRender_Emitter::priority				()
{
	float	dist		= SoundRender->listener_position().distance_to	(p_source.position);
	float	att			= p_source.min_distance/(psSoundRolloff*dist);	clamp(att,0.f,1.f);
	return	smooth_volume*att*priority_scale;
}

void	CSoundRender_Emitter::update_environment	(float dt)
{
	if (bMoved)			e_target	= *SoundRender->get_environment	(p_source.position);
	e_current.lerp		(e_current,e_target,dt);
}
