#include "stdafx.h"
#pragma hdrstop

#include "xrLevel.h"
#include "soundrender_core.h"
#include "soundrender_source.h"
#include "soundrender_emitter.h"
#include "soundrender_target.h"

CSoundRender_Core				SoundRender;
CSound_manager_interface*		Sound		= &SoundRender;

int		psSoundTargets			= 12;
Flags32	psSoundFlags			= {ssWaveTrace | ssHardware | ssFX};
float	psSoundOcclusionScale	= 0.5f;
float	psSoundCull				= 0.01f;
float	psSoundRolloff			= 0.25f;
float	psSoundDoppler			= 0.3f;
u32		psSoundFreq				= 0;
u32		psSoundModel			= 0;
float	psSoundVMaster			= 1.0f;
float	psSoundVEffects			= 1.0f;
float	psSoundVMusic			= 0.7f;
int		psSoundCacheSizeMB		= 16;

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
	s_targets_pu				= 0;
	s_emitters_u				= 0;
    e_current.set_identity		();
    e_target.set_identity		();
    bListenerMoved				= FALSE;
}

CSoundRender_Core::~CSoundRender_Core()
{
}
/*
BOOL QuerySupport(LPKSPROPERTYSET pExtensions, ULONG ulQuery, DWORD& m_dwSupport)
{
    ULONG ulSupport = 0;
    HRESULT hr 		= pExtensions->QuerySupport(DSPROPSETID_EAX20_ListenerProperties, ulQuery, &ulSupport);
    if ( FAILED(hr) ) return FALSE;
 
    if ( (ulSupport&(KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET)) == (KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET) )
    {
        m_dwSupport |= (DWORD)(1 << ulQuery); 
        return TRUE;
    }
 
    return FALSE;
}
*/
void CSoundRender_Core::_initialize	(u64 window)
{
	bPresent			= FALSE;
	if (strstr			( Core.Params,"-nosound"))		return;
	Timer.Start			( );

	// Device
	if( FAILED			( EAXDirectSoundCreate8( NULL, &pDevice, NULL ) ) )
		if( FAILED		( DirectSoundCreate8( NULL, &pDevice, NULL ) ) )	return;
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

	// Initialize listener data
	Listener.dwSize				= sizeof(DS3DLISTENER);
	Listener.vPosition.set		( 0.0f, 0.0f, 0.0f );
	Listener.vVelocity.set		( 0.0f, 0.0f, 0.0f );
	Listener.vOrientFront.set	( 0.0f, 0.0f, 1.0f );
	Listener.vOrientTop.set		( 0.0f, 1.0f, 0.0f );
	Listener.fDistanceFactor	= 1.0f;
	Listener.fRolloffFactor		= DS3D_DEFAULTROLLOFFFACTOR;
	Listener.fDopplerFactor		= DS3D_DEFAULTDOPPLERFACTOR;

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
        	BOOL bEAXres		= TRUE;
			if (FAILED(pTempBuf->QueryInterface(IID_IKsPropertySet, (LPVOID *)&pExtensions))){
                bEAXres			= FALSE;
            }else{
                ULONG support	= 0;
                if (FAILED(pExtensions->QuerySupport(DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ALLPARAMETERS, &support)))
	                bEAXres		= FALSE;
                if ((support & (KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET)) != (KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET))
	                bEAXres		= FALSE;
            }
            if (!bEAXres){
                Log				("EAX 2.0 not supported");
                _RELEASE		(pExtensions);
            }
        }
        _RELEASE				(pTempBuf);
    }

    // load environment
	env_load					();

	bPresent					=	TRUE;

	// Pre-create targets
	CSoundRender_Target*	T	= 0;
	for (u32 tit=0; tit<u32(psSoundTargets); tit++)
	{
		T							=	xr_new<CSoundRender_Target>();
		T->_initialize				();	
		s_targets.push_back			(T);
	}

	// Cache
	u32		bytes_per_line		= (sdef_target_block/4)*wfm.nAvgBytesPerSec/1000;
    cache.initialize			(psSoundCacheSizeMB*1024,bytes_per_line);
}



void CSoundRender_Core::_destroy	()
{
	cache.destroy				();
	env_unload					();

    // remove sources
	for (u32 sit=0; sit<s_sources.size(); sit++)
    	xr_delete				(s_sources[sit]);
    
    // remove emmiters
	for (u32 eit=0; eit<s_emitters.size(); eit++)
    	xr_delete				(s_emitters[eit]);

    // remove targets
	CSoundRender_Target*	T	= 0;
	for (u32 tit=0; tit<s_targets.size(); tit++)
	{
		T						= s_targets[tit];
		T->_destroy				();
        xr_delete				(T);
	}
    
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
	xr_vector<u16>			ids;
	IReader*				names	= I->open_chunk(0);
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
		u16		id_front	= (u16)((T->dummy&0x0000ffff)>>0);		//	front face
		u16		id_back		= (u16)((T->dummy&0xffff0000)>>16);		//	back face
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

void	CSoundRender_Core::create				( ref_sound& S, BOOL _3D, const char* fName, int type )
{
	if (!bPresent)					return;

	string256	fn;
	strcpy		(fn,fName);
	char*		E = strext(fn);
	if (E)		*E = 0;
	S.handle	= i_create_source				(fn,_3D);
	S.g_type	= (type==st_SourceType)?S.handle->game_type():type;
}

void	CSoundRender_Core::play					( ref_sound& S, CObject* O, u32 flags, float delay)
{
	if (!bPresent || 0==S.handle)	return;

	S.g_object		= O;
	if (S.feedback)	
	{
		CSoundRender_Emitter* E = (CSoundRender_Emitter*)S.feedback;
		E->rewind	();
	}	
	else			i_play	(&S,flags&sm_Looped,delay);
	if (flags&sm_2D)		S.feedback->switch_to_2D();
}
void	CSoundRender_Core::play_unlimited		( ref_sound& S, CObject* O, u32 flags, float delay)
{
	if (!bPresent || 0==S.handle) return;
	i_play					(&S,flags&sm_Looped,delay);
	if (flags&sm_2D)		S.feedback->switch_to_2D();
}
void	CSoundRender_Core::play_at_pos			( ref_sound& S, CObject* O, const Fvector &pos, u32 flags, float delay)
{
	if (!bPresent || 0==S.handle) return;
	S.g_object		= O;
	if (S.feedback)	
	{
		CSoundRender_Emitter* E = (CSoundRender_Emitter*)S.feedback;
		E->rewind	();
	}	
	else				i_play				(&S,flags&sm_Looped,delay);
	S.feedback->set_position				(pos);
	if (flags&sm_2D)			S.feedback->switch_to_2D();
}
void	CSoundRender_Core::play_at_pos_unlimited	( ref_sound& S, CObject* O, const Fvector &pos, u32 flags, float delay)
{
	if (!bPresent || 0==S.handle) return;
	i_play						(&S,flags&sm_Looped,delay);
	S.feedback->set_position	(pos);
	if (flags&sm_2D)			S.feedback->switch_to_2D();
}
void	CSoundRender_Core::destroy(ref_sound& S )
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
				Fvector*			V	= geom_ENV->get_verts();
				Fvector tri_norm;
				tri_norm.mknormal		(V[T->verts[0]],V[T->verts[1]],V[T->verts[2]]);
				float	dot				= dir.dotproduct(tri_norm);
				if (dot<0)
				{
					u16		id_front	= (u16)((T->dummy&0x0000ffff)>>0);		//	front face
					return	s_environment->Get(id_front);
				} else {
					u16		id_back		= (u16)((T->dummy&0xffff0000)>>16);	//	back face
					return	s_environment->Get(id_back);
				}
			} else
			{
				identity.set_identity	();
				return &identity;
			}
		} else
		{
			identity.set_identity	();
			return &identity;
		}
	}
}

void						CSoundRender_Core::env_apply		()
{
/*
	// Force all sounds to change their environment
	// (set their positions to signal changes in environment)
	for (u32 it=0; it<s_emitters.size(); it++)
	{
		CSoundRender_Emitter*	pEmitter	= s_emitters[it];
		const CSound_params*	pParams		= pEmitter->get_params	();
		pEmitter->set_position	(pParams->position);
	}
*/
    bListenerMoved			= TRUE;
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
void						CSoundRender_Core::refresh_sources()
{
	for (u32 sit=0; sit<s_sources.size(); sit++){
    	CSoundRender_Source* s = s_sources[sit];
        s->unload		();
//.		if (FS.exist(*s->fname)) s->load(*s->fname,s->_3D); // не правильно так искать
        s->load(*s->fname,s->_3D);
    }
}
#endif


