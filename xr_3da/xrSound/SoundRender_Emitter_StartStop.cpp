#include "stdafx.h"
#pragma hdrstop

#include "SoundRender_Core.h"
#include "SoundRender_Emitter.h"
#include "SoundRender_Source.h"

void CSoundRender_Emitter::start(ref_sound* _owner, BOOL _loop, float delay)
{
	starting_delay			= delay;
	source					= (CSoundRender_Source*)_owner->handle;
	owner					= _owner;
	p_source.position.set	(0,0,0);
	p_source.min_distance	= source->m_fMinDist;	// DS3D_DEFAULTMINDISTANCE;
	p_source.max_distance	= source->m_fMaxDist;	// 300.f;
	p_source.volume			= source->m_fVolume; 	// 1.f
	p_source.freq			= 1.f;

    if (fis_zero(delay,EPS_L)){
		state				= _loop?stStartingLooped:stStarting;
    }else{
		state				= _loop?stStartingLoopedDelayed:stStartingDelayed;
    }
}

void CSoundRender_Emitter::stop	()
{
	if (target)	SoundRender->i_stop		(this);
	if (owner)	
	{
		Event_ReleaseOwner		();
		owner->feedback			= NULL;
		owner					= NULL;
	}
	state = stStopped;
}

void CSoundRender_Emitter::rewind()
{
	u32 dwTime					=	SoundRender->Timer.GetElapsed_ms();
	u32 dwDiff					=	dwTime-dwTimeStarted;
	dwTimeStarted				+=	dwDiff;
	dwTimeToStop				+=	dwDiff;
	dwTimeToPropagade			=	dwTime;

	position					=	0;
	if (target)	SoundRender->i_rewind	(this);
}

void CSoundRender_Emitter::cancel()
{
	// Msg		("- %10s : %3d[%1.4f] : %s","cancel",dbg_ID,priority(),source->fname);
	switch	(state) 
	{
	case stPlaying:
		// switch to: SIMULATE
		state					=	stSimulating;		// switch state
		SoundRender->i_stop		(this);
		break;
	case stPlayingLooped:
		// switch to: SIMULATE
		state					=	stSimulatingLooped;	// switch state
		SoundRender->i_stop		(this);
		break;
	default:
		Debug.fatal	("Non playing ref_sound forced out of render queue");
		break;
	}
}
