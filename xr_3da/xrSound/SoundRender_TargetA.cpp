#include "stdafx.h"
#pragma hdrstop

#include "soundrender_TargetA.h"
#include "soundrender_emitter.h"
#include "soundrender_source.h"

static xr_vector<u8> buf_temp_data;

CSoundRender_TargetA::CSoundRender_TargetA():CSoundRender_Target()
{
    cache_gain			= 0.f;
    cache_pitch			= 1.f;
}

CSoundRender_TargetA::~CSoundRender_TargetA()
{
}

void	CSoundRender_TargetA::_initialize		()
{
	inherited::_initialize();
    // initialize buffer
	A_CHK(alGenBuffers	(sdef_target_count, pBuffers));	
	A_CHK(alGenSources	(1, &pSource));                	
	A_CHK(alSourcei		(pSource, AL_LOOPING, AL_FALSE));
    A_CHK(alSourcef		(pSource, AL_MIN_GAIN, 0.f));
    A_CHK(alSourcef		(pSource, AL_MAX_GAIN, 1.f));
	A_CHK(alSourcef		(pSource, AL_GAIN, 	cache_gain));
	A_CHK(alSourcef		(pSource, AL_PITCH,	cache_pitch));
}

void	CSoundRender_TargetA::_destroy		()
{
	// clean up target
	A_CHK(alDeleteSources	(1, &pSource));
	A_CHK(alDeleteBuffers	(sdef_target_count, pBuffers));
}

void	CSoundRender_TargetA::start			(CSoundRender_Emitter* E)
{
    inherited::start(E);

	// Calc storage
	buf_block			= sdef_target_block*wfx.nAvgBytesPerSec/1000;
    buf_temp_data.resize(buf_block);
}

void	CSoundRender_TargetA::render		()
{
	for (u32 buf_idx=0; buf_idx<sdef_target_count; buf_idx++)
		fill_block	(pBuffers[buf_idx]);

	A_CHK			(alSourceQueueBuffers	(pSource, sdef_target_count, pBuffers));	
	A_CHK			(alSourcePlay			(pSource));

    inherited::render();
}

void	CSoundRender_TargetA::stop			()
{
	if (rendering)
	{
		A_CHK		(alSourceStop(pSource));
        A_CHK		(alSourcei	(pSource, AL_BUFFER,   NULL));
		A_CHK		(alSourcei	(pSource, AL_SOURCE_RELATIVE,	FALSE));
	}
    inherited::stop	();
}

void	CSoundRender_TargetA::rewind			()
{
	inherited::rewind();
	A_CHK			(alSourceStop(pSource));
	A_CHK			(alSourcei	(pSource, AL_BUFFER,   NULL));
	for (u32 buf_idx=0; buf_idx<sdef_target_count; buf_idx++)
		fill_block	(pBuffers[buf_idx]);
	A_CHK			(alSourceQueueBuffers	(pSource, sdef_target_count, pBuffers));	
	A_CHK			(alSourcePlay			(pSource));
}

void	CSoundRender_TargetA::update			()
{
	inherited::update();

	ALint			processed;
    // Get status
    A_CHK			(alGetSourcei(pSource, AL_BUFFERS_PROCESSED, &processed));

    if (processed > 0){
        while (processed){
			ALuint	BufferID;
            A_CHK	(alSourceUnqueueBuffers(pSource, 1, &BufferID));
            fill_block(BufferID);
            A_CHK	(alSourceQueueBuffers(pSource, 1, &BufferID));
            processed--;
        }
    }else{ 
    	// processed == 0
        // check play status -- if stopped then queue is not being filled fast enough
        ALint state;
	    A_CHK		(alGetSourcei(pSource, AL_SOURCE_STATE, &state));
        if (state != AL_PLAYING){
//			Log		("Queuing underrun detected.");
			A_CHK	(alSourcePlay(pSource));
        }
    }
}

void	CSoundRender_TargetA::fill_parameters()
{
	inherited::fill_parameters();

    // 3D params
    A_CHK(alSourcef	(pSource, AL_REFERENCE_DISTANCE, 	pEmitter->p_source.min_distance));
    A_CHK(alSourcef	(pSource, AL_MAX_DISTANCE, 			pEmitter->p_source.max_distance));
	if (pEmitter->b2D){
		A_CHK(alSource3f(pSource, AL_POSITION,	 		0.f,0.f,0.f));
	}else{
		A_CHK(alSource3f(pSource, AL_POSITION,	 		pEmitter->p_source.position.x,pEmitter->p_source.position.y,-pEmitter->p_source.position.z));
	}
    A_CHK(alSourcei	(pSource, AL_SOURCE_RELATIVE,		pEmitter->b2D));
    // 2D params
    A_CHK(alSourcef	(pSource, AL_ROLLOFF_FACTOR,		psSoundRolloff));
    float	_gain	= pEmitter->smooth_volume;			clamp	(_gain,EPS_S,1.f);
    if (!fsimilar(_gain,cache_gain)){
        cache_gain	= _gain;
        A_CHK(alSourcef	(pSource, AL_GAIN,				_gain));
    }
    float	_pitch	= pEmitter->p_source.freq;			clamp	(_pitch,EPS_S,2.f);
    if (!fsimilar(_pitch,cache_pitch)){
        cache_pitch	= _pitch;
        A_CHK(alSourcef	(pSource, AL_PITCH,				_pitch));
    }
}

void	CSoundRender_TargetA::fill_block	(ALuint BufferID)
{
#pragma todo("check why pEmitter is NULL")
	if (0==pEmitter)	return;

	pEmitter->fill_block(&buf_temp_data.front(),buf_block);

	ALuint format 		= (wfx.nChannels==1)?AL_FORMAT_MONO16:AL_FORMAT_STEREO16;
    A_CHK				(alBufferData(BufferID, format, &buf_temp_data.front(), buf_block, wfx.nSamplesPerSec));
}
