#include "stdafx.h"
#pragma hdrstop

#include "xrLevel.h"
#include "soundrender_core.h"
#include "soundrender_source.h"
#include "soundrender_emitter.h"
#include "soundrender_target.h"
 
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

	env_load					();

	bPresent					=	TRUE;

	// Pre-create targets
	CSoundRender_Target*	T	= 0;
	for (u32 tit=0; tit<32; tit++)
	{
		T							=	xr_new<CSoundRender_Target>();
		T->_initialize				();	
		s_targets.push_back			(T);
	}
}

void CSoundRender_Core::_destroy	()
{
	env_unload					();

	_RELEASE		( pExtensions	);
	_RELEASE		( pListener		);
	_RELEASE		( pBuffer		);
	_RELEASE		( pDevice		);
}

void CSoundRender_Core::env_load	()
{
	// Load environment
	string256					fn;
	if (FS.exist(fn,"$game_data$",SNDENV_FILENAME))
	{
		s_environment				= xr_new<SoundEnvironment_LIB>();
		s_environment->Load			(fn);
	}

	// Load geometry

	// Assosiate geometry
}

void CSoundRender_Core::env_unload	()
{
	// Unload 
	if (s_environment)
		s_environment->Unload	();
	xr_delete					(s_environment);

	// Unload geometry
}

void CSoundRender_Core::_restart		()
{
	env_apply					();
}

void CSoundRender_Core::set_geometry_occ(CDB::MODEL* M)
{
	geom_MODEL		= M;
}

void CSoundRender_Core::set_handler(sound_event* E)
{
	Handler			= E;
}

void CSoundRender_Core::set_geometry_env(IReader* I)
{
	xr_delete				(geom_ENV);
	if (0==I)				return;
	if (0==s_environment)	return;

	// Assosiate names
	vector<u16>			ids;
	IReader*			names	= I->open_chunk(0);
	while (!names->eof())
	{
		string256			n;
		names->r_stringZ	(n);
		int id				= s_environment->GetID(n);
		R_ASSERT			(id>=0);
		ids.push_back		(u16(id));
	}
	names->close		();

	// Load geometry
	IReader*			geom	= I->open_chunk(1);
	hdrCFORM			H;
	geom->r				(&H,sizeof(hdrCFORM));
	Fvector*	verts	= (Fvector*)geom->pointer();
	CDB::TRI*	tris	= (CDB::TRI*)(verts+H.vertcount);
	for (u32 it=0; it<H.facecount; it++)
	{
		CDB::TRI*	T		= tris+it;
		u16		id_front	= (T->dummy&0x0000ffff)>>0;		//	front face
		u16		id_back		= (T->dummy&0xffff0000)>>16;	//	back face
		R_ASSERT			(id_front<ids.size());
		R_ASSERT			(id_back<ids.size());
		T->dummy			= u32(ids[id_back]<<16) | u32(ids[id_front]);
	}
	geom_ENV			= xr_new<CDB::MODEL> ();
	geom_ENV->build		(verts, H.vertcount, tris, H.facecount );
	geom->close			();

#ifdef _EDITOR
	env_apply			();
#endif
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
	R_ASSERT			(0==S.feedback);
	i_destroy_source	((CSoundRender_Source*)S.handle);
	S.handle			= NULL;
}

CSoundRender_Environment*	CSoundRender_Core::get_environment			( Fvector& P )
{
	static CSoundRender_Environment	identity;

	if (bUserEnvironment)
	{
		return &s_user_environment;
	} 
	else 
	{
		if (geom_ENV)
		{
			geom_DB.ray_options		(CDB::OPT_ONLYNEAREST);
			Fvector	dir				= {0,-1,0};
			geom_DB.ray_query		(geom_ENV,P,dir,1000.f);
			if (geom_DB.r_count())
			{
				CDB::RESULT*		r	= geom_DB.r_begin();
				CDB::TRI*			T	= geom_ENV->get_tris()+r->id;
				Fvector tri_norm;
				tri_norm.mknormal		(*T->verts[0],*T->verts[1],*T->verts[2]);
				float	dot				= dir.dotproduct(tri_norm);
				if (dot<0)
				{
					u16		id_front	= (T->dummy&0x0000ffff)>>0;		//	front face
					return	s_environment->Get(id_front);
				} else {
					u16		id_back		= (T->dummy&0xffff0000)>>16;	//	back face
					return	s_environment->Get(id_back);
				}
			} else
			{
				identity.set_identity	(true,true,true);
				return &identity;
			}
		} else
		{
			identity.set_identity	(true,true,true);
			return &identity;
		}
	}
}

void						CSoundRender_Core::env_apply		()
{
	// Force all sounds to change their environment
	// (set their positions to signal changes in environment)
	for (u32 it=0; it<s_emitters.size(); it++)
	{
		CSoundRender_Emitter*	pEmitter	= s_emitters[it];
		const CSound_params*	pParams		= pEmitter->get_params	();
		pEmitter->set_position	(pParams->position);
	}
}

#ifdef _EDITOR
void						CSoundRender_Core::set_user_env		( CSound_environment* E)
{
	if (0==E && !bUserEnvironment)	return;

	if (E)
	{
		s_user_environment	= *((CSoundRender_Environment*)E);
		bUserEnvironment	= TRUE;
	}
	else 
	{
		bUserEnvironment	= FALSE;
	}
	env_apply			();
}

void						CSoundRender_Core::refresh_env_library()
{
	env_unload			();
	env_load			();
	env_apply			();
}
#endif
