#include "stdafx.h"
#pragma hdrstop

#include "soundrender_core.h"
#include "soundrender_source.h"
#include "soundrender_emitter.h"
 
CSoundRender_Core				SoundRender;
CSound_manager_interface*		Sound		= &SoundRender;

int		psSoundRelaxTime		= 10;
Flags32	psSoundFlags			= {ssWaveTrace | ssSoftware};
float	psSoundOcclusionScale	= 0.85f;
float	psSoundCull				= 0.01f;
float	psSoundRolloff			= 0.25f;
float	psSoundDoppler			= 0.3f;
u32		psSoundFreq				= 0;
u32		psSoundModel			= 0;
float	psSoundVMaster			= 1.0f;
float	psSoundVEffects			= 1.0f;
float	psSoundVMusic			= 0.7f;

CSoundRender_Core::CSoundRender_Core	()
{
	bPresent					= FALSE;
	bUserEnvironment			= FALSE;
	pDevice						= NULL;
	pBuffer						= NULL;
	pListener					= NULL;
	pExtensions					= NULL;
	geom_MODEL					= NULL;
	geom_ENV					= NULL;
	s_environment				= NULL;
	Handler						= NULL;
}

CSoundRender_Core::~CSoundRender_Core()
{
}

void CSoundRender_Core::_initialize	(u32 window)
{
	bPresent		= FALSE;

	if (strstr			( Core.Params,"-nosound"))		return;
	Timer.Start			( );

	// Device
	if( FAILED			( DirectSoundCreate8( NULL, &pDevice, NULL ) ) )					return;
	if( FAILED			( pDevice->SetCooperativeLevel(  (HWND)window, DSSCL_PRIORITY ) ) )	
	{
		_destroy();
		return;
	}

	// Create primary buffer.
	DSBUFFERDESC		dsbd;
	WAVEFORMATEX		wfm;
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
	DSCAPS						dsCaps;
	dsCaps.dwSize				= sizeof(DSCAPS);
	R_ASSERT					(pDevice);
	R_CHK						(pDevice->GetCaps (&dsCaps));

	ZeroMemory					( &wfm, sizeof( WAVEFORMATEX ) );
	switch	( psSoundFreq )
	{
	case sf_11K:	wfm.nSamplesPerSec = 11025; break;
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

	// Initialize listener data
	Listener.dwSize				= sizeof(DS3DLISTENER);
	Listener.vPosition.set		( 0.0f, 0.0f, 0.0f );
	Listener.vVelocity.set		( 0.0f, 0.0f, 0.0f );
	Listener.vOrientFront.set	( 0.0f, 0.0f, 1.0f );
	Listener.vOrientTop.set		( 0.0f, 1.0f, 0.0f );
	Listener.fDistanceFactor	= 1.0f;
	Listener.fRolloffFactor		= DS3D_DEFAULTROLLOFFFACTOR;
	Listener.fDopplerFactor		= DS3D_DEFAULTDOPPLERFACTOR;

	// Load environment
	string256					fn;
	if (FS.exist(fn,"$game_data$",SNDENV_FILENAME))
	{
		s_environment				= xr_new<SoundEnvironment_LIB>();
		s_environment->Load			(fn);
	}

	bPresent					= TRUE;
}

void CSoundRender_Core::_destroy	()
{
	_RELEASE		( pExtensions	);
	_RELEASE		( pListener		);
	_RELEASE		( pBuffer		);
	_RELEASE		( pDevice		);
}
void CSoundRender_Core::_restart	()
{
}

void CSoundRender_Core::set_geometry(CDB::MODEL* M)
{
	geom_MODEL		= M;
}

void CSoundRender_Core::set_handler(sound_event* E)
{
	Handler			= E;
}

void	CSoundRender_Core::create				( sound& S, BOOL _3D, const char* fName, int type )
{
	if (!bPresent)					return;

	string256	fn;
	strcpy		(fn,fName);
	char*		E = strext(fn);
	if (E)		*E = 0;
	S.handle	= i_create_source				(fn,_3D);
	S.g_type	= type;
}

void	CSoundRender_Core::play					( sound& S, CObject* O, BOOL bLoop)
{
	if (!bPresent || 0==S.handle)	return;

	S.g_object		= O;
	if (S.feedback)	
	{
		CSoundRender_Emitter* E = (CSoundRender_Emitter*)S.feedback;
		E->rewind	();
	}	
	else			i_play	(&S,bLoop);
}
void	CSoundRender_Core::play_unlimited		( sound& S, CObject* O, BOOL bLoop)
{
	if (!bPresent || 0==S.handle) return;
	i_play				(&S,bLoop);
}
void	CSoundRender_Core::play_at_pos			( sound& S, CObject* O, const Fvector &pos, BOOL bLoop)
{
	if (!bPresent || 0==S.handle) return;
	S.g_object		= O;
	if (S.feedback)	
	{
		CSoundRender_Emitter* E = (CSoundRender_Emitter*)S.feedback;
		E->rewind	();
	}	
	else				i_play				(&S,bLoop);
	S.feedback->set_position				(pos);
}
void	CSoundRender_Core::play_at_pos_unlimited	( sound& S, CObject* O, const Fvector &pos, BOOL bLoop)
{
	if (!bPresent || 0==S.handle) return;
	i_play						(&S,bLoop);
	S.feedback->set_position	(pos);
}
void	CSoundRender_Core::destroy(sound& S )
{
	if (!bPresent || 0==S.handle) {
		S.handle	= 0;
		S.feedback	= 0;
		return;
	}
	if (S.feedback)		
	{
		CSoundRender_Emitter* E = (CSoundRender_Emitter*)S.feedback;
		E->stop					();
	}
	i_destroy_source	((CSoundRender_Source*)S.handle);
}

CSoundRender_Environment*	CSoundRender_Core::get_environment			( Fvector& P )
{
	if (bUserEnvironment)
	{
		return &s_user_environment;
	} else 
	{
		static CSoundRender_Environment	identity;
		identity.set_identity	(true,true,true);
		// identity.set_default	(false,false,false);
		return &identity;
	}
}

void						CSoundRender_Core::set_user_environment		( CSound_environment* E)
{
	if (E)
	{
		s_user_environment	= *((CSoundRender_Environment*)E);
		bUserEnvironment	= TRUE;

		// Force all sounds to change their environment
		// (set their positions to signal changes in environment)
		for (u32 it=0; it<s_emitters.size(); it++)
		{
			CSoundRender_Emitter*	pEmitter	= s_emitters[it];
			const CSound_params*	pParams		= pEmitter->get_params	();
			pEmitter->set_position	(pParams->position);
		}
	}
	else 
	{
		bUserEnvironment	= FALSE;
	}
}
