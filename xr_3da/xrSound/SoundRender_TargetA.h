#ifndef SoundRender_TargetH
#define SoundRender_TargetH
#pragma once

#include "soundrender_Target.h"

class CSoundRender_Target
{
// OpenAL
    ALuint						pSource;
	ALuint						pBuffers[sdef_target_count];
    float						cache_gain;
    float						cache_pitch;

// DirectX
	WAVEFORMATEX				wfx;
	IDirectSoundBuffer*			pBuffer_base;
	IDirectSoundBuffer8*		pBuffer;
	IDirectSound3DBuffer8*		pControl;

	BOOL						bDX7;

	u32							buf_time;		// ms
	u32							buf_size;		// bytes
	u32							buf_block;

	s32							cache_hw_volume;
	s32							cache_hw_freq;

	CSoundRender_Emitter*		pEmitter;
	u32							pos_write;		// bytes
	BOOL						rendering;
public:
	float						priority;
private:
	BOOL						isDX7			() { return bDX7; };
	void						fill_block		();
	void						fill_block_al	(ALuint BufferID);
	u32							calc_interval	(u32 ptr);
public:
	void						_initialize		();
	void						_destroy		();

	CSoundRender_Emitter*		get_emitter		()	{ return pEmitter;	}
	BOOL						get_Rendering	()	{ return rendering;	}

	void						start			(CSoundRender_Emitter* E);
	void						render			();
	void						rewind			();
	void						stop			();
	void						update			();
	void						fill_parameters	();

	CSoundRender_Target(void);
	~CSoundRender_Target(void);
};
#endif