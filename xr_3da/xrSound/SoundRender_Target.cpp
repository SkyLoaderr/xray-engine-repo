#include "stdafx.h"
#pragma hdrstop

#include "soundrender.h"
#include "soundrender_core.h"
#include "soundrender_target.h"
#include "soundrender_emitter.h"

CSoundRender_Target::CSoundRender_Target(void)
{
	pBuffer_base	= NULL;
	pBuffer			= NULL;
	pControl		= NULL;

	pFX_Reverb		= NULL;
	pFX_Echo		= NULL;
	pFX_Distortion	= NULL;

	cache_hw_volume	= DSBVOLUME_MIN;
	cache_hw_freq	= 11025;

	pEmitter		= 0;
	pos_write		= 0;
	rendering		= FALSE;
}

CSoundRender_Target::~CSoundRender_Target(void)
{
}

void	CSoundRender_Target::_initialize		()
{
	// Calc format
	WAVEFORMATEX			wfx_primary;
	SoundRender.pBuffer->GetFormat(&wfx_primary,sizeof(wfx_primary),0);
	wfx.wFormatTag			= WAVE_FORMAT_PCM;
	wfx.nChannels			= 1;
	wfx.nSamplesPerSec		= wfx_primary.nSamplesPerSec;
	wfx.wBitsPerSample		= 16;
	wfx.nBlockAlign			= wfx.nChannels * wfx.wBitsPerSample / 8;
	wfx.nAvgBytesPerSec		= wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize				= 0;

	// Calc storage
	buf_time				= sdef_target_size;	
	buf_size				= sdef_target_size*wfx.nAvgBytesPerSec/1000;
	buf_block				= sdef_target_block*wfx.nAvgBytesPerSec/1000;

	// Fill caps structure
	DSBUFFERDESC	dsBD	= {0};
	dsBD.dwSize				= sizeof(dsBD);
	dsBD.dwFlags			= 
		DSBCAPS_CTRL3D | 
		DSBCAPS_CTRLFREQUENCY | 
		DSBCAPS_CTRLFX | 
		DSBCAPS_CTRLVOLUME |
		DSBCAPS_GETCURRENTPOSITION2 |
		DSBCAPS_LOCSOFTWARE;
	dsBD.dwBufferBytes		= buf_size;
	dsBD.dwReserved			= 0;
	dsBD.lpwfxFormat		= &wfx;

	switch (psSoundModel) 
	{
	case sq_DEFAULT:	dsBD.guid3DAlgorithm = DS3DALG_DEFAULT; 			break;
	case sq_NOVIRT:		dsBD.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION; 	break;
	case sq_LIGHT:		dsBD.guid3DAlgorithm = DS3DALG_HRTF_LIGHT;			break;
	case sq_HIGH:		dsBD.guid3DAlgorithm = DS3DALG_HRTF_FULL;			break;
	default:			Debug.fatal("Unknown 3D-sound algorithm");			break;
	}
	dsBD.guid3DAlgorithm	= DS3DALG_DEFAULT;

	// Create
	R_CHK	(SoundRender.pDevice->CreateSoundBuffer(&dsBD, &pBuffer_base, NULL));
	R_CHK	(pBuffer_base->QueryInterface(IID_IDirectSoundBuffer8,(void **)&pBuffer));
	R_CHK	(pBuffer_base->QueryInterface(IID_IDirectSound3DBuffer8,(void **)&pControl));
	R_ASSERT(pBuffer_base && pBuffer && pControl);

	// DMOs
	DSEFFECTDESC	desc	[2];
	desc[0].dwSize			= sizeof(DSEFFECTDESC);
	desc[0].dwFlags			= DSFX_LOCSOFTWARE;
	desc[0].guidDSFXClass	= GUID_DSFX_WAVES_REVERB;
	desc[0].dwReserved1		= 0;
	desc[0].dwReserved2		= 0;

	desc[1].dwSize			= sizeof(DSEFFECTDESC);
	desc[1].dwFlags			= DSFX_LOCSOFTWARE;
	desc[1].guidDSFXClass	= GUID_DSFX_STANDARD_ECHO;
	desc[1].dwReserved1		= 0;
	desc[1].dwReserved2		= 0;

	R_CHK	(pBuffer->SetFX(2,desc,NULL));
	R_CHK	(pBuffer->GetObjectInPath(desc[0].guidDSFXClass, 0, IID_IDirectSoundFXWavesReverb8,(void**)&pFX_Reverb));
	R_CHK	(pBuffer->GetObjectInPath(desc[1].guidDSFXClass, 0, IID_IDirectSoundFXEcho8,		(void**)&pFX_Echo));
}

void	CSoundRender_Target::_destroy		()
{
	_RELEASE(pFX_Distortion);
	_RELEASE(pFX_Echo);
	_RELEASE(pFX_Reverb);
	_RELEASE(pControl);
	_RELEASE(pBuffer);
	_RELEASE(pBuffer_base);
}

void	CSoundRender_Target::start			(CSoundRender_Emitter* E)
{
	R_ASSERT		(E);

	// *** Initial buffer startup ***
	// 1. Fill parameters
	// 4. Load 2 blocks of data (as much as possible)
	// 5. Deferred-play-signal (emitter-exist, rendering-false)
	pEmitter		= E;
	pos_write		= 0;
	rendering		= FALSE;
}

void	CSoundRender_Target::render			()
{
	fill_block		();
	fill_block		();

	R_CHK			(pBuffer->SetCurrentPosition	(0));
	R_CHK			(pBuffer->Play(0,0,DSBPLAY_LOOPING));
	rendering		= TRUE;
}

void	CSoundRender_Target::stop			()
{
	if (rendering)
	{
		R_CHK			(pBuffer->Stop());
		R_CHK			(pControl->SetMode(DS3DMODE_DISABLE,DS3D_DEFERRED));
	}
	pEmitter		= NULL;
	rendering		= FALSE;
}

void	CSoundRender_Target::rewind			()
{
	R_ASSERT(rendering);
	u32		pos_2_play	= pos_write%buf_size;
	fill_parameters		();
	fill_block			();
	R_CHK	(pBuffer->SetCurrentPosition	(pos_2_play));
}

u32		CSoundRender_Target::calc_interval	(u32 ptr)
{
	u32		norm_ptr	= ptr%buf_size;
	u32		range		= norm_ptr/buf_block;
	return	range;
}

void	CSoundRender_Target::update			()
{
	R_ASSERT		(pEmitter);

	// fill_parameters	();

	// Analyze if we really need more data to stream them ahead
	u32				cursor_write;
	R_CHK			(pBuffer->GetCurrentPosition(0,LPDWORD(&cursor_write)));

	u32				r_write		= calc_interval(pos_write);
	u32				r_cursor	= (calc_interval(cursor_write)+1)%3;
	if (r_write==r_cursor)		fill_block	();
}

void	CSoundRender_Target::fill_parameters()
{
	VERIFY			(pEmitter);

	// 1. Set 3D params (including mode)
	{
		Fvector&					p_pos	= pEmitter->p_source.position;
		DS3DBUFFER					buf;
		buf.dwSize					= sizeof(DS3DBUFFER);
		buf.vPosition.x				= p_pos.x;
		buf.vPosition.y				= p_pos.y;
		buf.vPosition.z				= p_pos.z;
		buf.vVelocity.x				= 0;
		buf.vVelocity.y				= 0;
		buf.vVelocity.z				= 0;
		buf.vConeOrientation.x		= 0;
		buf.vConeOrientation.y		= 0;
		buf.vConeOrientation.z		= 1;
		buf.dwInsideConeAngle		= DS3D_DEFAULTCONEANGLE;
		buf.dwOutsideConeAngle		= DS3D_DEFAULTCONEANGLE;
		buf.lConeOutsideVolume		= DS3D_DEFAULTCONEOUTSIDEVOLUME;
		buf.flMinDistance			= pEmitter->p_source.min_distance;
		buf.flMaxDistance			= pEmitter->p_source.max_distance;
		buf.dwMode					= DS3DMODE_NORMAL;
		R_CHK(pControl->SetAllParameters(&buf,DS3D_DEFERRED));
	}
	
	// 2. Set 2D params (volume, freq) + position(rewind)
	{
		float	_volume				= pEmitter->smooth_volume;				clamp	(_volume,EPS_S,1.f);
		s32		hw_volume			= iFloor	(7000.f*logf(_volume)/5.f);	clamp	(hw_volume,DSBVOLUME_MIN,DSBVOLUME_MAX);
		if (_abs(hw_volume-cache_hw_volume)>50)
		{
			cache_hw_volume				= hw_volume;
			R_CHK(pBuffer->SetVolume	( hw_volume	));
		}

		float	_freq				= pEmitter->p_source.freq;
		s32		hw_freq				= iFloor	(_freq * float(wfx.nSamplesPerSec) + EPS);
		if (_abs(hw_freq-cache_hw_freq)>50)
		{
			cache_hw_freq				= hw_freq;
			R_CHK(pBuffer->SetFrequency	( hw_freq	));
		}
	}
	
	// 3. Set FX params (environment)
	{
		CSoundRender_Environment& E		= pEmitter->e_current;

		// 1. Reverb
		{
			DSFXWavesReverb					para;
			para.fInGain					= E.R_InGain;
			para.fReverbMix					= E.R_Mix;
			para.fReverbTime				= E.R_Time;
			para.fHighFreqRTRatio			= E.R_HFRatio;
			pFX_Reverb->SetAllParameters	(&para);
		}

		// 2. Echo
		{
			DSFXEcho						para;
			para.fWetDryMix					= E.E_WetDry;
			para.fFeedback					= E.E_FeedBack;
			para.fLeftDelay					= E.E_Delay;
			para.fRightDelay				= E.E_Delay;
			para.lPanDelay					= FALSE;
			pFX_Echo->SetAllParameters		(&para);
		}
	}
}

void	CSoundRender_Target::fill_block		()
{
	VERIFY			(pEmitter);

	// Obtain memory address of write block. This will be in two parts if the block wraps around.
    LPVOID			ptr1, ptr2;
    u32				bytes1,bytes2;
    R_CHK			(pBuffer->Lock(pos_write%buf_size, buf_block, &ptr1, LPDWORD(&bytes1), &ptr2, LPDWORD(&bytes2), 0));
	R_ASSERT		(0==ptr2 && 0==bytes2);

	// Copy data (and clear the end)
	pEmitter->fill_block(ptr1,buf_block);
	pos_write		+=	buf_block;

	R_CHK			(pBuffer->Unlock(ptr1, bytes1, ptr2, bytes2));
}
