#include "stdafx.h"
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
	state = stStopped;
}

void CSoundRender_Emitter::rewind()
{
	u32 dwTime					=	Device.TimerAsync();
	u32 dwDiff					=	dwTime-dwTimeStarted;
	dwTimeStarted				+=	dwDiff;
	dwTimeToStop				+=	dwDiff;
	dwTimeToPropagade			=	dwTime;

	position					=	0;
	if (target)	SoundRender.i_rewind	(this);
}
