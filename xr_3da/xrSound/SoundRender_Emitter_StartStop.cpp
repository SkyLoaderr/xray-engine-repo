#include "stdafx.h"
#pragma hdrstop

#include "SoundRender_Core.h"
#include "SoundRender_Emitter.h"
#include "SoundRender_Source.h"

void CSoundRender_Emitter::start(
								 sound*							_owner, 
								 BOOL							_loop
								 )
{
	source					= (CSoundRender_Source*)_owner->handle;
	owner					= _owner;
	p_source.position.set	(0,0,0);
	p_source.min_distance	= DS3D_DEFAULTMINDISTANCE;
	p_source.max_distance	= 300.f;
	p_source.freq			= 1.f;
	p_source.volume			= 1.f;

	state					= _loop?stStartingLooped:stStarting;
}

void CSoundRender_Emitter::stop	()
{
	if (target)	SoundRender.i_stop		(this);
	if (owner)	
	{
		owner->feedback			= NULL;
		owner					= NULL;
	}
	state = stStopped;
}

void CSoundRender_Emitter::rewind()
{
	u32 dwTime					=	SoundRender.Timer.GetElapsed_ms();
	u32 dwDiff					=	dwTime-dwTimeStarted;
	dwTimeStarted				+=	dwDiff;
	dwTimeToStop				+=	dwDiff;
	dwTimeToPropagade			=	dwTime;

	position					=	0;
	if (target)	SoundRender.i_rewind	(this);
}

void CSoundRender_Emitter::cancel()
{
	Msg		("- %10s : %3d[%1.4f] : %s","cancel",dbg_ID,priority(),source->fname);
	switch	(state) 
	{
	case stPlaying:
		// switch to: SIMULATE
		state					=	stSimulating;		// switch state
		SoundRender.i_stop		(this);
		break;
	case stPlayingLooped:
		// switch to: SIMULATE
		state					=	stSimulatingLooped;	// switch state
		SoundRender.i_stop		(this);
		break;
	default:
		R_ASSERT2	(0, "Non playing sound forced out of render queue");
		break;
	}
}
