#include "stdafx.h"
#include "SoundRender_Emitter.h"
#include "SoundRender_Core.h"
#include "SoundRender_Source.h"

void CSoundRender_Emitter::update	()
{
	u32	dwTime			= Device.TimerAsync	();

	switch (state)	
	{
	case stStopped:
		break;
	case stStarting:
		dwTimeStarted		= dwTime;
		dwTimeToStop		= dwTime + source->dwTimeTotal;
		dwTimeToPropagade	= dwTime;
		occluder_volume		= (SoundRender.get_occlusion	(p_source.position,.2f,occluder))?0.f:1.f;
		smooth_volume		= p_source.volume*psSoundVEffects*(occluder_volume*(1.f-psSoundOcclusionScale)+psSoundOcclusionScale);
		e_current = e_target= *SoundRender.get_environment	(p_source.position);
		if (update_culling())	
		{
			state				=	stPlaying;
			position			=	0;
			SoundRender.i_start(this);
		}
		else state				=	stSimulating;
		break;
	case stStartingLooped:
		dwTimeStarted		= dwTime;
		dwTimeToStop		= 0xffffffff;
		dwTimeToPropagade	= dwTime;
		occluder_volume		= (SoundRender.get_occlusion	(p_source.position,.2f,occluder))?0.f:1.f;
		smooth_volume		= p_source.volume*psSoundVEffects*(occluder_volume*(1.f-psSoundOcclusionScale)+psSoundOcclusionScale);
		e_current = e_target= *SoundRender.get_environment	(p_source.position);
		if (update_culling())	
		{
			state				=	stPlayingLooped;
			position			=	0;
			SoundRender.i_start(this);
		}
		else state				=	stSimulatingLooped;
		break;
	case stPlaying:
		if (dwTime>=dwTimeToStop)	
		{
			// STOP
			state					=	stStopped;
			SoundRender.i_stop		(this);
		} else {
			if (!update_culling()) {
				// switch to: SIMULATE
				state					=	stSimulating;		// switch state
				SoundRender.i_stop		(this);
			}
			else 
			{
				// We are still playing
				update_environment	();
			}
		}
		break;
	case stSimulating:
		if (dwTime>=dwTimeToStop)	
		{
			// STOP
			state					=	stStopped;
		} else {
			if (update_culling()) {
				// switch to: PLAY
				state					=	stPlaying;
				position				= 	(((dwTime-dwTimeStarted)%source->dwTimeTotal)*source->dwBytesPerMS);
				SoundRender.i_start	(this);
			}
		}
		break;
	case stPlayingLooped:
		if (!update_culling()) 
		{
			// switch to: SIMULATE
			state					=	stSimulatingLooped;	// switch state
			SoundRender.i_stop		(this);
		}
		else 
		{
			// We are still playing
			update_environment	();
		}
		break;
	case stSimulatingLooped:
		if (update_culling()) 
		{
			// switch to: PLAY
			state					=	stPlayingLooped;	// switch state
			position				= (((dwTime-dwTimeStarted)%source->dwTimeTotal)*source->dwBytesPerMS);
			SoundRender.i_start	(this);
		}
		break;
	}

	// footer
	bMoved				= FALSE;
	if (state != stStopped)
	{
		if (dwTime	>=	dwTimeToPropagade)		PropagadeEvent();
	} else if (owner)	{ owner->feedback = 0; owner	= 0; }
}

BOOL	CSoundRender_Emitter::update_culling	()
{
	// Check range
	float	dist		= SoundRender.Listener.distance_to	(p_source.position);
	if (dist>p_source.max_distance)	return FALSE;

	// Calc attenuated volume
	float att			= p_source.min_distance/(psSoundRolloff*dist);	clamp(att,0.f,1.f);
	if (att*p_source.volume*psSoundVEffects<psSoundCull)				return FALSE;

	// Update occlusion
	float occ			=	(SoundRender.get_occlusion	(p_source.position,.2f,occluder))?-1.f:1.f;
	occluder_volume		+=	Device.fTimeDelta*2*occ;
	clamp				(occluder_volume,0.f,1.f);

	// Update smoothing
	float vol_occ		=	occluder_volume*(1.f-psSoundOcclusionScale)+psSoundOcclusionScale;
	smooth_volume		=	.9f*smooth_volume + .1f*(p_source.volume*psSoundVEffects*vol_occ);
	if (smooth_volume*att<psSoundCull)	return FALSE;
	return TRUE;
}

void	CSoundRender_Emitter::update_environment	()
{
	if (bMoved)			e_target	= *SoundRender.get_environment	(p_source.position);
	e_current.lerp		(e_current,e_target,Device.fTimeDelta);
}
