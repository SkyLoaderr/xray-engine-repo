#include "stdafx.h"
#pragma hdrstop

#ifdef OPENAL

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
    // Get the device sp        ecifier.
//      deviceSpecifier         	= alcGetString		(pDevice, ALC_DEVICE_SPECIFIER);
//		Msg				        	("OpenAL: Using device '%s'.", deviceSpecifier);
    // Create context
    ALCcontext*	pContext        = alcCreateContext	(pDevice,NULL);
    // Set active contex        t
    A_CHK				        (alcMakeContextCurrent(pContext));

    // initialize listen        er
    A_CHK				        (alListener3f		(AL_POSITION,0.f,0.f,0.f));
    A_CHK				        (alListener3f		(AL_VELOCITY,0.f,0.f,0.f));
    Fvector	orient[2]	        = {{0.f,0.f,1.f},{0.f,1.f,0.f}};
    A_CHK				        (alListenerfv		(AL_ORIENTATION,&orient[0].x));
    A_CHK				        (alListenerf		(AL_GAIN,psSoundVMaster));

    // Check for EAX ext        ension
    bEAX 				        = alIsExtensionPresent("EAX");

    eaxSet 				        = (EAXSet)alGetProcAddress("EAXSet");
    if (eaxSet==NULL) bEAX 		= false;
    eaxGet 				        = (EAXGet)alGetProcAddress("EAXGet");
    if (eaxGet==NULL) bEAX 		= false;

	ZeroMemory					( &wfm, sizeof( WAVEFORMATEX ) );
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

void	CSoundRender_CoreA::i_set_eax			(CSound_environment* _E)
{
	VERIFY(bEAX);
    CSoundRender_Environment* E = static_cast<CSoundRender_Environment*>(_E);
    EAXLISTENERPROPERTIES 		ep;
    ep.lRoom					= iFloor(E->Room)				;	// room effect level at low frequencies
    ep.lRoomHF					= iFloor(E->RoomHF)				;   // room effect high-frequency level re. low frequency level
    ep.flRoomRolloffFactor		= E->RoomRolloffFactor			;   // like DS3D flRolloffFactor but for room effect
    ep.flDecayTime				= E->DecayTime					;   // reverberation decay time at low frequencies
    ep.flDecayHFRatio			= E->DecayHFRatio				;   // high-frequency to low-frequency decay time ratio
    ep.lReflections				= iFloor(E->Reflections)		;   // early reflections level relative to room effect
    ep.flReflectionsDelay		= E->ReflectionsDelay			;   // initial reflection delay time
    ep.lReverb					= iFloor(E->Reverb)	 			;   // late reverberation level relative to room effect
    ep.flReverbDelay			= E->ReverbDelay				;   // late reverberation delay time relative to initial reflection
    ep.dwEnvironment			= EAXLISTENER_DEFAULTENVIRONMENT;  	// sets all listener properties
    ep.flEnvironmentSize		= E->EnvironmentSize			;  	// environment size in meters
    ep.flEnvironmentDiffusion	= E->EnvironmentDiffusion		; 	// environment diffusion
    ep.flAirAbsorptionHF		= E->AirAbsorptionHF			;	// change in level per meter at 5 kHz
    ep.dwFlags					= EAXLISTENER_DEFAULTFLAGS		;	// modifies the behavior of properties

    A_CHK(eaxSet	      	    (&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ROOM, 					NULL, 0, ep.lRoom					));
    A_CHK(eaxSet          	    (&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ROOMHF, 				NULL, 0, ep.lRoomHF					));
    A_CHK(eaxSet          	    (&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ROOMROLLOFFFACTOR, 	NULL, 0, ep.flRoomRolloffFactor		));
    A_CHK(eaxSet          	    (&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_DECAYTIME, 			NULL, 0, ep.flDecayTime				));
    A_CHK(eaxSet          	    (&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_DECAYHFRATIO,			NULL, 0, ep.flDecayHFRatio			));
    A_CHK(eaxSet          	    (&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_REFLECTIONS, 			NULL, 0, ep.lReflections			));
    A_CHK(eaxSet          	    (&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_REFLECTIONSDELAY, 		NULL, 0, ep.flReflectionsDelay		));
    A_CHK(eaxSet          	    (&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_REVERB, 				NULL, 0, ep.lReverb					));
    A_CHK(eaxSet          	    (&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_REVERBDELAY, 			NULL, 0, ep.flReverbDelay			));
    A_CHK(eaxSet          	    (&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENTDIFFUSION,	NULL, 0, ep.flEnvironmentDiffusion	));
    A_CHK(eaxSet          	    (&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF, 		NULL, 0, ep.flAirAbsorptionHF		));
    A_CHK(eaxSet          	    (&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_FLAGS, 				NULL, 0, ep.dwFlags				));
}

void	CSoundRender_CoreA::i_get_eax			(CSound_environment* _E)
{
	VERIFY(bEAX);
    CSoundRender_Environment* E = static_cast<CSoundRender_Environment*>(_E);
    EAXLISTENERPROPERTIES 		ep;
    A_CHK(eaxGet      			(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ALLPARAMETERS,NULL, &ep, sizeof(EAXLISTENERPROPERTIES)));
    E->Room						= ep.lRoom					;
    E->RoomHF					= ep.lRoomHF				;
    E->RoomRolloffFactor		= ep.flRoomRolloffFactor	;
    E->DecayTime			   	= ep.flDecayTime			;
    E->DecayHFRatio				= ep.flDecayHFRatio			;
    E->Reflections				= ep.lReflections			;
    E->ReflectionsDelay			= ep.flReflectionsDelay		;
    E->Reverb					= ep.lReverb				;
    E->ReverbDelay				= ep.flReverbDelay			;
    E->EnvironmentSize			= ep.flEnvironmentSize		;
    E->EnvironmentDiffusion		= ep.flEnvironmentDiffusion	;
    E->AirAbsorptionHF			= ep.flAirAbsorptionHF		;
}

void CSoundRender_CoreA::update_listener( const Fvector& P, const Fvector& D, const Fvector& N, float dt )
{
	inherited::update_listener(P,D,N,dt);

    A_CHK						(alListener3f		(AL_POSITION,Listener.vPosition.x,Listener.vPosition.y,-Listener.vPosition.z));
    A_CHK						(alListener3f		(AL_VELOCITY,0.f,0.f,0.f));
    Fvector	orient[2]			= {{D.x,D.y,-D.z},{N.x,N.y,-N.z}};
    A_CHK						(alListenerfv		(AL_ORIENTATION,&orient[0].x));
}

#ifdef _EDITOR
void CSoundRender_CoreA::set_environment_size(CSound_environment* src_env, CSound_environment** dst_env)
{
	if (bEAX){
    	CSoundRender_Environment* SE 	= static_cast<CSoundRender_Environment*>(src_env); 
    	CSoundRender_Environment* DE 	= static_cast<CSoundRender_Environment*>(*dst_env); 
    	// set environment
        A_CHK(eaxSet		    		(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENTSIZE, NULL, 0, SE->EnvironmentSize));
//        R_CHK(pExtensions->Set			(DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENTSIZE, NULL, 0, &SE->EnvironmentSize, sizeof(float)));
        i_set_eax						(SE);
        A_CHK(eaxSet		    		(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENTSIZE, NULL, 0, DE->EnvironmentSize));
//        R_CHK(pExtensions->Set			(DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENTSIZE, NULL, 0, &DE->EnvironmentSize, sizeof(float)));
        i_get_eax						(DE);
    }
}
void CSoundRender_CoreA::set_environment(u32 id, CSound_environment** dst_env)
{
	if (bEAX){
    	CSoundRender_Environment* DE 	= static_cast<CSoundRender_Environment*>(*dst_env); 
    	// set environment
        A_CHK(eaxSet		    		(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENTSIZE, NULL, 0, id));
//        R_CHK(pExtensions->Set			(DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENT, NULL, 0, &id, sizeof(id)));
        i_get_eax						(DE);
    }
}
#endif
#endif
