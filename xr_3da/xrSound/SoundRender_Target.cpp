#include "stdafx.h"
#pragma hdrstop

#include "soundrender_target.h"
#include "soundrender_core.h"

CSoundRender_Target::CSoundRender_Target(void)
{
	pEmitter		= 0;
	rendering		= FALSE;
}

CSoundRender_Target::~CSoundRender_Target(void)
{
}

BOOL CSoundRender_Target::_initialize()
{
	// Calc format
	wfx.wFormatTag			= WAVE_FORMAT_PCM;
	wfx.nChannels			= 1;
	wfx.nSamplesPerSec		= SoundRender->wfm.nSamplesPerSec;
	wfx.wBitsPerSample		= 16;
	wfx.nBlockAlign			= wfx.nChannels * wfx.wBitsPerSample / 8;
	wfx.nAvgBytesPerSec		= wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize				= 0;
    return					TRUE;
}

void	CSoundRender_Target::start			(CSoundRender_Emitter* E)
{
	R_ASSERT		(E);

	// *** Initial buffer startup ***
	// 1. Fill parameters
	// 4. Load 2 blocks of data (as much as possible)
	// 5. Deferred-play-signal (emitter-exist, rendering-false)
	pEmitter		= E;
	rendering		= FALSE;
}

void	CSoundRender_Target::render			()
{
	rendering		= TRUE;
}

void	CSoundRender_Target::stop			()
{
	pEmitter		= NULL;
	rendering		= FALSE;
}

void	CSoundRender_Target::rewind			()
{
	R_ASSERT		(rendering);
}

void	CSoundRender_Target::update			()
{
	R_ASSERT		(pEmitter);
}

void	CSoundRender_Target::fill_parameters()
{
	VERIFY			(pEmitter);
}
