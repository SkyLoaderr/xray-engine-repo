#include "stdafx.h"
#pragma hdrstop

#ifndef OPENAL

#include "soundrender_coreD.h"
#include "soundrender_targetD.h"

CSoundRender_CoreD				SoundRenderD;
CSoundRender_Core*				SoundRender = &SoundRenderD;
CSound_manager_interface*		Sound		= SoundRender;

CSoundRender_CoreD::CSoundRender_CoreD	():CSoundRender_Core()
{
	pDevice						= NULL;
	pBuffer						= NULL;
	pListener					= NULL;
	pExtensions					= NULL;
}

CSoundRender_CoreD::~CSoundRender_CoreD()
{
}

void CSoundRender_CoreD::_initialize	(u64 window)
{
	bPresent			= FALSE;
	if (strstr			( Core.Params,"-nosound"))		return;

	// DirectX device
	if( FAILED			( EAXDirectSoundCreate8( NULL, &pDevice, NULL ) ) )
		if( FAILED		( DirectSoundCreate8( NULL, &pDevice, NULL ) ) )	return;
	if( FAILED			( pDevice->SetCooperativeLevel(  (HWND)window, DSSCL_PRIORITY ) ) )	
	{
		_destroy();
		return;
	}

	// Create primary buffer.
	DSBUFFERDESC		dsbd;
	ZeroMemory			( &dsbd, sizeof( DSBUFFERDESC ) );
	dsbd.dwSize			= sizeof( DSBUFFERDESC );
	dsbd.dwFlags		= DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes	= 0;
	if( FAILED	( pDevice->CreateSoundBuffer( &dsbd, &pBuffer, NULL ) ) )
	{
		_destroy();
		return;
	}

	// Calculate primary buffer format.
	dsCaps.dwSize				= sizeof(DSCAPS);
	R_ASSERT					(pDevice);
	R_CHK						(pDevice->GetCaps (&dsCaps));

	ZeroMemory					( &wfm, sizeof( WAVEFORMATEX ) );
	switch	( psSoundFreq )
	{
	default:
	case sf_22K:	wfm.nSamplesPerSec = 22050; break;
	case sf_44K:	wfm.nSamplesPerSec = 44100; break;
	}
	wfm.wFormatTag				= WAVE_FORMAT_PCM;
	wfm.nChannels				= (dsCaps.dwFlags&DSCAPS_PRIMARYSTEREO)?2:1;
	wfm.wBitsPerSample			= (dsCaps.dwFlags&DSCAPS_PRIMARY16BIT)?16:8;
	wfm.nBlockAlign				= wfm.wBitsPerSample / 8 * wfm.nChannels;
	wfm.nAvgBytesPerSec			= wfm.nSamplesPerSec * wfm.nBlockAlign;

	// For safety only :)
	R_CHK(pBuffer->SetFormat	(&wfm));
	R_CHK(pBuffer->Play			(0,0,DSBPLAY_LOOPING));

	// Get listener interface.
	R_CHK(pBuffer->QueryInterface( IID_IDirectSound3DListener8, (VOID**)&pListener ));
	R_ASSERT					(pListener);

    // Create property set
	if (psSoundFlags.test(ssHardware)){
        IDirectSoundBuffer*		pTempBuf;
        WAVEFORMATEX 			wave;
        Memory.mem_fill			(&wave, 0, sizeof(WAVEFORMATEX));
        wave.wFormatTag 		= WAVE_FORMAT_PCM;
        wave.nChannels 			= 1; 
        wave.nSamplesPerSec 	= 22050; 
        wave.wBitsPerSample 	= 16; 
        wave.nBlockAlign 		= wave.wBitsPerSample / 8 * wave.nChannels;
        wave.nAvgBytesPerSec	= wave.nSamplesPerSec * wave.nBlockAlign;

        DSBUFFERDESC 			desc;
        Memory.mem_fill			(&desc, 0, sizeof(DSBUFFERDESC));
        desc.dwSize 			= sizeof(DSBUFFERDESC); 
        desc.dwFlags 			= DSBCAPS_STATIC|DSBCAPS_CTRL3D; 
        desc.dwBufferBytes 		= 64;  
        desc.lpwfxFormat 		= &wave; 

		if (DS_OK==pDevice->CreateSoundBuffer(&desc, &pTempBuf, NULL)){
        	bEAX				= TRUE;
			if (FAILED(pTempBuf->QueryInterface(IID_IKsPropertySet, (LPVOID *)&pExtensions))){
                bEAX			= FALSE;
            }else{
                ULONG support	= 0;
                if (FAILED(pExtensions->QuerySupport(DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ALLPARAMETERS, &support)))
	                bEAX		= FALSE;
                if ((support & (KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET)) != (KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET))
	                bEAX		= FALSE;
            }
            if (!bEAX){
                Log				("EAX 2.0 not supported");
                _RELEASE		(pExtensions);
            }
        }
        _RELEASE				(pTempBuf);
    }

    // inherited initialize
    inherited::_initialize		(window);

	// Pre-create targets
	CSoundRender_Target*	T	= 0;
	for (u32 tit=0; tit<u32(psSoundTargets); tit++)
	{
		T						= xr_new<CSoundRender_TargetD>();
		T->_initialize			();	
		s_targets.push_back		(T);
	}
}

void CSoundRender_CoreD::_destroy	()
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
    // cleanup dx
	_RELEASE					( pExtensions	);
	_RELEASE					( pListener		);
	_RELEASE					( pBuffer		);
	_RELEASE					( pDevice		);
}

void CSoundRender_CoreD::i_set_eax		(CSound_environment* _E)
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
    R_CHK(pExtensions->Set		(DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ROOM, 					NULL, 0, &ep.lRoom, 				sizeof(LONG)));
    R_CHK(pExtensions->Set     	(DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ROOMHF, 				NULL, 0, &ep.lRoomHF, 				sizeof(LONG)));
    R_CHK(pExtensions->Set      (DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ROOMROLLOFFFACTOR, 		NULL, 0, &ep.flRoomRolloffFactor,	sizeof(float)));
    R_CHK(pExtensions->Set      (DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_DECAYTIME, 				NULL, 0, &ep.flDecayTime, 			sizeof(float)));
    R_CHK(pExtensions->Set      (DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_DECAYHFRATIO,			NULL, 0, &ep.flDecayHFRatio, 		sizeof(float)));
    R_CHK(pExtensions->Set      (DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_REFLECTIONS, 			NULL, 0, &ep.lReflections, 			sizeof(LONG)));
    R_CHK(pExtensions->Set      (DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_REFLECTIONSDELAY, 		NULL, 0, &ep.flReflectionsDelay, 	sizeof(float)));
    R_CHK(pExtensions->Set      (DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_REVERB, 				NULL, 0, &ep.lReverb, 				sizeof(LONG)));
    R_CHK(pExtensions->Set      (DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_REVERBDELAY, 			NULL, 0, &ep.flReverbDelay, 		sizeof(float)));
    R_CHK(pExtensions->Set      (DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENTDIFFUSION,	NULL, 0, &ep.flEnvironmentDiffusion,sizeof(float)));
    R_CHK(pExtensions->Set      (DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF, 		NULL, 0, &ep.flAirAbsorptionHF, 	sizeof(float)));
    R_CHK(pExtensions->Set      (DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_FLAGS, 					NULL, 0, &ep.dwFlags, 				sizeof(DWORD)));
}

void	CSoundRender_CoreD::i_get_eax			(CSound_environment* _E)
{
	VERIFY(bEAX);
    CSoundRender_Environment* E = static_cast<CSoundRender_Environment*>(_E);
    EAXLISTENERPROPERTIES ep;
    // get all params
    unsigned long total_bytes;
    R_CHK(pExtensions->Get		(DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ALLPARAMETERS, NULL, 0, &ep, sizeof(EAXLISTENERPROPERTIES), &total_bytes));
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

void CSoundRender_CoreD::update_listener( const Fvector& P, const Fvector& D, const Fvector& N, float dt )
{
	inherited::update_listener(P,D,N,dt);
	// apply listener params
    pListener->SetAllParameters		((DS3DLISTENER*)&Listener, DS3D_DEFERRED );
    // commit deffered settings
    pListener->CommitDeferredSettings	();
}

#ifdef _EDITOR
void CSoundRender_CoreD::set_environment_size(CSound_environment* src_env, CSound_environment** dst_env)
{
	if (bEAX){
    	CSoundRender_Environment* SE 	= static_cast<CSoundRender_Environment*>(src_env); 
    	CSoundRender_Environment* DE 	= static_cast<CSoundRender_Environment*>(*dst_env); 
    	// set all params                                                                                                          
        R_CHK(pExtensions->Set			(DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENTSIZE, NULL, 0, &SE->EnvironmentSize, sizeof(float)));
        i_set_eax						(SE);
        R_CHK(pExtensions->Set			(DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENTSIZE, NULL, 0, &DE->EnvironmentSize, sizeof(float)));
        i_get_eax						(DE);
    }
}
void CSoundRender_CoreD::set_environment(u32 id, CSound_environment** dst_env)
{
	if (pExtensions){
    	CSoundRender_Environment* DE 	= static_cast<CSoundRender_Environment*>(*dst_env); 
    	// set all params                                                                                                          
        R_CHK(pExtensions->Set			(DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENT, NULL, 0, &id, sizeof(id)));
        i_get_eax						(DE);
    }
}
#endif
#endif
