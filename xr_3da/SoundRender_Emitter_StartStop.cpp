#include "stdafx.h"
#include "SoundRender_Core.h"
#include "SoundRender_Emitter.h"
#include "SoundRender_Source.h"

void CSoundRender_Emitter::start(
								 sound*							_owner, 
								 CSoundRender_Source*			_source, 
								 CSoundRender_EmitterParams*	_params,
								 BOOL							_loop
								 )
{
	source		= dynamic_cast<CSoundRender_Source*>_source;
	owner		= _owner;
	p_source	= *_params;

	state		= stStarting;
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
