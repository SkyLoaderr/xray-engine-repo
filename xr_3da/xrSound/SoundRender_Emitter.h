#ifndef SoundRender_EmitterH
#define SoundRender_EmitterH
#pragma once

#include "soundrender.h"
#include "soundrender_environment.h"
#include "soundrender_emitter_params.h"

class CSoundRender_Emitter		:	public CSound_interface
{
	float						starting_delay;
public:
	enum State
	{
		stStopped		= 0,

		stStartingDelayed,
		stStartingLoopedDelayed,

		stStarting,
		stStartingLooped,

		stPlaying,
		stPlayingLooped,

		stSimulating,
		stSimulatingLooped,

		stFORCEDWORD	= u32(-1)
	};
public:
#ifdef DEBUG
	u32							dbg_ID;
#endif

	CSoundRender_Target*		target;
	CSoundRender_Source*		source;
	ref_sound*					owner;

	float						priority_scale;
	float						smooth_volume;
	float 						occluder_volume;		// USER
	Fvector						occluder	[3];

	State						state;
	u32							position;
	CSound_params				p_source;
	CSoundRender_Environment	e_current;
	CSoundRender_Environment	e_target;

	BOOL						bMoved;
	BOOL						b2D;
	u32							dwTimeStarted;			// time of "Start"
	u32							dwTimeToStop;			// time to "Stop"
	u32							dwTimeToPropagade;

	u32							marker;
public:
	void						Event_Propagade			();
	void						Event_ReleaseOwner		();
	BOOL						isPlaying				(void)					{ return state!=stStopped; }

	virtual void				switch_to_2D			()						{ b2D = TRUE;											}
	virtual void				switch_to_3D			()						{ b2D = FALSE;											}
	virtual void				set_position			(const Fvector &pos)	{ p_source.position	= pos; bMoved=TRUE;					}
	virtual void				set_frequency			(float scale)			{ p_source.freq=scale;									}
	virtual void				set_range				(float min, float max)	{ p_source.min_distance=min; p_source.max_distance=max;	}
	virtual void				set_volume				(float vol)				{ p_source.volume = vol;								}
	virtual void				set_priority			(float p)				{ priority_scale = p;									}
	virtual	const CSound_params* get_params				( )						{ return &p_source;										}

	void						fill_block				(void*	ptr, u32 size);
	void						fill_data				(u8*	ptr, u32 offset, u32 size);

	float						priority				();
	void						start					(ref_sound* _owner, BOOL _loop, float delay);
	void						cancel					();						// manager forces out of rendering
	void						update					(float dt);
	BOOL						update_culling			(float dt);
	void						update_environment		(float dt);
	void						rewind					();
	virtual void				stop					();

	CSoundRender_Emitter		();
	~CSoundRender_Emitter		();
};
#endif