#ifndef SoundRender_TargetH
#endif SoundRender_TargetH
#pragma once

#include "soundrender.h"

class CSoundRender_Target
{
	WAVEFORMATEX				wfx;
	IDirectSoundBuffer*			pBuffer_base;
	IDirectSoundBuffer8*		pBuffer;
	IDirectSound3DBuffer8*		pControl;

	IDirectSoundFXWavesReverb8*	pFX_Reverb;
	IDirectSoundFXEcho8*		pFX_Echo;
	IDirectSoundFXDistortion8*	pFX_Distortion;

	u32							buf_time;		// ms
	u32							buf_size;		// bytes
	u32							buf_block;

	s32							cache_hw_volume;
	s32							cache_hw_freq;

	CSoundRender_Emitter*		pEmitter;
	u32							pos_write;		// bytes
	BOOL						rendering;
private:
	void						fill_block		();
	void						fill_parameters	();
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

	CSoundRender_Target(void);
	~CSoundRender_Target(void);
};
#endif