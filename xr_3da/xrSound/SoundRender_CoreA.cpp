#include "stdafx.h"
#pragma hdrstop

#ifdef _OPENAL

#include "soundrender_coreA.h"
#include "soundrender_targetA.h"

CSoundRender_CoreA				SoundRenderA;                
CSoundRender_Core*				SoundRender = &SoundRenderA;
CSound_manager_interface*		Sound		= SoundRender;

CSoundRender_CoreA::CSoundRender_CoreA	():CSoundRender_Core()
{
    eaxSet						= 0;
    eaxGet						= 0;
}

CSoundRender_CoreA::~CSoundRender_CoreA	()
{
}

void CSoundRender_CoreA::_initialize	(u64 window)
{
	bPresent			        = FALSE;
	if (strstr			        ( Core.Params,"-nosound"))		return;

    // OpenAL device
    ALCubyte* 			        deviceSpecifier;
    ALCubyte 			        deviceName[] = "DirectSound3D";
    // OpenAL device
    ALCdevice*	pDevice	        = alcOpenDevice		(deviceName);
    // Get the device specifier.
#ifndef _EDITOR    
    deviceSpecifier         	= alcGetString		(pDevice, ALC_DEVICE_SPECIFIER);
	Msg				        	("OpenAL: Using device '%s'.", deviceSpecifier);
#endif    
    // Create context
    ALCcontext*	pContext        = alcCreateContext	(pDevice,NULL);
    // Set active context
    A_CHK				        (alcMakeContextCurrent(pContext));

    // initialize listener
    A_CHK				        (alListener3f		(AL_POSITION,0.f,0.f,0.f));
    A_CHK				        (alListener3f		(AL_VELOCITY,0.f,0.f,0.f));
    Fvector	orient[2]	        = {{0.f,0.f,1.f},{0.f,1.f,0.f}};
    A_CHK				        (alListenerfv		(AL_ORIENTATION,&orient[0].x));
    A_CHK				        (alListenerf		(AL_GAIN,psSoundVMaster));

    // Check for EAX extension
    bEAX 				        = alIsExtensionPresent		((ALubyte*)"EAX");

    eaxSet 				        = (EAXSet*)alGetProcAddress	((ALubyte*)"EAXSet");
    if (eaxSet==NULL) bEAX 		= false;
    eaxGet 				        = (EAXGet*)alGetProcAddress	((ALubyte*)"EAXGet");
    if (eaxGet==NULL) bEAX 		= false;

	ZeroMemory					( &wfm, sizeof( WAVEFORMATEX ) );
//.    
//	psSoundFreq = sf_22K;
//	psSoundFreq = sf_44K;
	switch	( psSoundFreq ){
	default:
	case sf_22K:	wfm.nSamplesPerSec = 22050; break;
	case sf_44K:	wfm.nSamplesPerSec = 44100; break;
	}
	wfm.wFormatTag				= WAVE_FORMAT_PCM;
	wfm.nChannels				= 2;	//(dsCaps.dwFlags&DSCAPS_PRIMARYSTEREO)?2:1;
	wfm.wBitsPerSample			= 16;	//(dsCaps.dwFlags&DSCAPS_PRIMARY16BIT)?16:8;
	wfm.nBlockAlign				= wfm.wBitsPerSample / 8 * wfm.nChannels;
	wfm.nAvgBytesPerSec			= wfm.nSamplesPerSec * wfm.nBlockAlign;

    // inherited initialize           
    inherited::_initialize		(window);

	// Pre-create targets
	CSoundRender_Target*	T	= 0;
	for (u32 tit=0; tit<u32(psSoundTargets); tit++)
	{
		T						=	xr_new<CSoundRender_TargetA>();
		T->_initialize			();	
		s_targets.push_back		(T);
	}
}

void CSoundRender_CoreA::_destroy	()
{
	inherited::_destroy			();
    // remove targets
	CSoundRender_Target*	T	= 0;
	for (u32 tit=0; tit<s_targets.size(); tit++)
	{
		T						= s_targets[tit];
		T->_destroy				();
        xr_delete				(T);
	}
    // Get the current context.
    ALCcontext* pCurContext		= alcGetCurrentContext();
    // Get the device used by that context.
    ALCdevice* 	pCurDevice		= alcGetContextsDevice(pCurContext);
    // Reset the current context to NULL.
    alcMakeContextCurrent		(NULL);         
    // Release the context and the device.
    alcDestroyContext			(pCurContext);
    alcCloseDevice				(pCurDevice);
}

void	CSoundRender_CoreA::i_eax_set			(const GUID* guid, u32 prop, void* val, u32 sz)
{
	A_CHK(eaxSet	      	    (guid, prop, 0, val, sz));
}
void	CSoundRender_CoreA::i_eax_get			(const GUID* guid, u32 prop, void* val, u32 sz)
{
	A_CHK(eaxGet	      	    (guid, prop, 0, val, sz));
}

void CSoundRender_CoreA::update_listener		( const Fvector& P, const Fvector& D, const Fvector& N, float dt )
{
	inherited::update_listener(P,D,N,dt);

	if (!Listener.position.similar(P)){
		Listener.position.set	(P);
		bListenerMoved			= TRUE;
	}
	Listener.orientation[0].set	(D.x,D.y,-D.z);
	Listener.orientation[1].set	(N.x,N.y,-N.z);

	A_CHK						(alListener3f	(AL_POSITION,Listener.position.x,Listener.position.y,-Listener.position.z));
    A_CHK						(alListener3f	(AL_VELOCITY,0.f,0.f,0.f));
    A_CHK						(alListenerfv	(AL_ORIENTATION,&Listener.orientation[0].x));
    A_CHK				        (alListenerf	(AL_GAIN,psSoundVMaster));
}
#endif
