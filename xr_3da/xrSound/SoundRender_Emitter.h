#pragma once

#include "soundrender.h"
#include "soundrender_environment.h"
#include "soundrender_emitter_params.h"

class CSoundRender_Emitter		:	public CSound_interface
{
public:
	enum State
	{
		stStopped		= 0,

		stStarting,
		stStartingLooped,

		stPlaying,
		stPlayingLooped,

		stSimulating,
		stSimulatingLooped,

		stFORCEDWORD	= u32(-1)
	};
public:
	CSoundRender_Target*		target;
	CSoundRender_Source*		source;
	sound*						owner;

	float						smooth_volume;
	float 						occluder_volume;		// USER
	Fvector						occluder	[3];

	State						state;
	u32							position;
	CSoundRender_EmitterParams	p_source;
	CSoundRender_Environment	e_current;
	CSoundRender_Environment	e_target;

	BOOL						bMoved;
	u32							dwTimeStarted;			// time of "Start"
	u32							dwTimeToStop;			// time to "Stop"
	u32							dwTimeToPropagade;
public:
	void						PropagadeEvent			();
	BOOL						isPlaying				(void)					{ return state!=stStopped; }

	virtual void				set_position			(const Fvector &pos)	{ p_source.position	= pos; bMoved=TRUE;					}
	virtual void				set_frequency			(float scale)			{ p_source.freq=scale;									}
	virtual void				set_range				(float min, float max)	{ p_source.min_distance=min; p_source.max_distance=max;	}
	virtual void				set_volume				(float vol)				{ p_source.volume = vol;								}

	void						fill_block				(void* ptr, u32 size);

	void						start					(sound* _owner, BOOL _loop);
	void						update					();
	BOOL						update_culling			();
	void						update_environment		();
	void						rewind					();
	virtual void				stop					();

	CSoundRender_Emitter		();
	~CSoundRender_Emitter		();
};
