#include "stdafx.h"
#pragma hdrstop

#include "xrLevel.h"
#include "soundrender_core.h"
#include "soundrender_source.h"
#include "soundrender_emitter.h"

int		psSoundTargets			= 16;
Flags32	psSoundFlags			= {ssHardware | ssEAX};
float	psSoundOcclusionScale	= 0.5f;
float	psSoundCull				= 0.01f;
float	psSoundRolloff			= 0.75f;
u32		psSoundFreq				= 0;
u32		psSoundModel			= 0;
float	psSoundVEffects			= 1.0f;
float	psSoundVFactor			= 1.0f;

float	psSoundVMusic			= 0.7f;
int		psSoundCacheSizeMB		= 16;

CSoundRender_Core*				SoundRender = 0;
CSound_manager_interface*		Sound		= 0;

CSoundRender_Core::CSoundRender_Core	()
{
	bPresent					= FALSE;
    bEAX						= FALSE;
    bDeferredEAX				= FALSE;
	bUserEnvironment			= FALSE;
	geom_MODEL					= NULL;
	geom_ENV					= NULL;
	geom_SOM					= NULL;
	s_environment				= NULL;
	Handler						= NULL;
	s_targets_pu				= 0;
	s_emitters_u				= 0;
    e_current.set_identity		();
    e_target.set_identity		();
    bListenerMoved				= FALSE;
    bReady						= FALSE;
    bLocked						= FALSE;
}

CSoundRender_Core::~CSoundRender_Core()
{
#ifdef _EDITOR
	ETOOLS::destroy_model		(geom_ENV);
	ETOOLS::destroy_model		(geom_SOM);
#else
	xr_delete					(geom_ENV);
	xr_delete					(geom_SOM);
#endif
}

void CSoundRender_Core::_initialize	(u64 window)
{
    Log							("* sound: EAX 2.0 extension:",bEAX?"present":"absent");
    Log							("* sound: EAX 2.0 deferred:",bDeferredEAX?"present":"absent");
	Timer.Start					( );

    // load environment
	env_load					();

	bPresent					= TRUE;

	// Cache
	cache_bytes_per_line		= (sdef_target_block/8)*wfm.nAvgBytesPerSec/1000;
    cache.initialize			(psSoundCacheSizeMB*1024,cache_bytes_per_line);

    bReady						= TRUE;
}

void CSoundRender_Core::_clear	()
{
    bReady						= FALSE;
	cache.destroy				();
	env_unload					();

    // remove sources
	for (u32 sit=0; sit<s_sources.size(); sit++)
    	xr_delete				(s_sources[sit]);
    s_sources.clear				();
    
    // remove emmiters
	for (u32 eit=0; eit<s_emitters.size(); eit++)
    	xr_delete				(s_emitters[eit]);
    s_emitters.clear			();
}

void CSoundRender_Core::stop_emitters()
{
	for (u32 eit=0; eit<s_emitters.size(); eit++)
		s_emitters[eit]->stop	(FALSE);
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
	cache.destroy				();
	cache.initialize			(psSoundCacheSizeMB*1024,cache_bytes_per_line);
	env_apply					();
}

void CSoundRender_Core::set_handler(sound_event* E)
{
	Handler			= E;
}

void CSoundRender_Core::set_geometry_occ(CDB::MODEL* M)
{
	geom_MODEL		= M;
}

void CSoundRender_Core::set_geometry_som(IReader* I)
{
#ifdef _EDITOR
	ETOOLS::destroy_model	(geom_SOM);
#else
	xr_delete				(geom_SOM);
#endif
	if (0==I)		return;

	// check version
	R_ASSERT		(I->find_chunk(0));
	u32 version		= I->r_u32(); 
	VERIFY2			(version==0,"Invalid SOM version");
	// load geometry	
	IReader* geom	= I->open_chunk(1); 
	VERIFY2			(geom,"Corrupted SOM file");
	// Load tris and merge them
	struct SOM_poly{
		Fvector3	v1;
		Fvector3	v2;
		Fvector3	v3;
		u32			b2sided;
		float		occ;
	};
	// Create AABB-tree
#ifdef _EDITOR    
	CDB::Collector*	CL			= ETOOLS::create_collector();
	while (!geom->eof()){
		SOM_poly				P;
		geom->r					(&P,sizeof(P));
        ETOOLS::collector_add_face_pd		(CL,P.v1,P.v2,P.v3,*(u32*)&P.occ,0.01f);
		if (P.b2sided)
			ETOOLS::collector_add_face_pd	(CL,P.v3,P.v2,P.v1,*(u32*)&P.occ,0.01f);
	}
	geom_SOM					= ETOOLS::create_model_cl(CL);
    ETOOLS::destroy_collector	(CL);
#else
	CDB::Collector				CL;			
	while (!geom->eof()){
		SOM_poly				P;
		geom->r					(&P,sizeof(P));
		CL.add_face_packed_D	(P.v1,P.v2,P.v3,*(u32*)&P.occ,0.01f);
		if (P.b2sided)
			CL.add_face_packed_D(P.v3,P.v2,P.v1,*(u32*)&P.occ,0.01f);
	}
	geom_SOM			= xr_new<CDB::MODEL> ();
	geom_SOM->build		(CL.getV(),int(CL.getVS()),CL.getT(),int(CL.getTS()));
#endif
}

void CSoundRender_Core::set_geometry_env(IReader* I)
{
#ifdef _EDITOR
	ETOOLS::destroy_model	(geom_ENV);
#else
	xr_delete				(geom_ENV);
#endif
	if (0==I)				return;
	if (0==s_environment)	return;

	// Assosiate names
	xr_vector<u16>			ids;
	IReader*				names	= I->open_chunk(0);
	while (!names->eof())
	{
		string256			n;
		names->r_stringZ	(n,sizeof(n));
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
		CDB::TRI*	T	= tris+it;
		u16		id_front= (u16)((T->dummy&0x0000ffff)>>0);		//	front face
		u16		id_back	= (u16)((T->dummy&0xffff0000)>>16);		//	back face
		R_ASSERT		(id_front<(u16)ids.size());
		R_ASSERT		(id_back<(u16)ids.size());
		T->dummy		= u32(ids[id_back]<<16) | u32(ids[id_front]);
	}
#ifdef _EDITOR    
	geom_ENV			= ETOOLS::create_model(verts, H.vertcount, tris, H.facecount);
	env_apply			();
#else
	geom_ENV			= xr_new<CDB::MODEL> ();
	geom_ENV->build		(verts, H.vertcount, tris, H.facecount);
#endif
	geom->close			();
}

void	CSoundRender_Core::verify_refsound		( ref_sound& S)
{
#ifdef	DEBUG
	int			local_value		= 0;
	void*		ptr_refsound	= &S;
	void*		ptr_local		= &local_value;
	ptrdiff_t	difference		= (ptrdiff_t)_abs(s64(ptrdiff_t(ptr_local) - ptrdiff_t(ptr_refsound)));
	VERIFY2		(difference > (64*1024), "local/stack-based ref_sound passed. memory corruption will accur.");
#endif
}

void	CSoundRender_Core::create				( ref_sound& S, BOOL _3D, const char* fName, int type )
{
	if (!bPresent)		return;
	verify_refsound		(S);
    S._p				= xr_new<ref_sound_data>(_3D,fName,type);
}

void	CSoundRender_Core::clone				( ref_sound& S, const ref_sound& from, int	type )
{
	if (!bPresent)		return;
	S._p				= xr_new<ref_sound_data>(*from._p);
	S._p->feedback		= 0;
	S._p->g_object		= 0;
	S._p->g_type		= (type==st_SourceType)?S._p->handle->game_type():type;
}


void	CSoundRender_Core::play					( ref_sound& S, CObject* O, u32 flags, float delay)
{
	if (!bPresent || 0==S._handle())return;
	verify_refsound		(S);
	S._p->g_object		= O;
	if (S._feedback())	((CSoundRender_Emitter*)S._feedback())->rewind ();
	else				i_play					(&S,flags&sm_Looped,delay);
	if (flags&sm_2D)	S._feedback()->switch_to_2D();
}
void	CSoundRender_Core::play_no_feedback		( ref_sound& S, CObject* O, u32 flags, float delay, Fvector* pos, float* vol, float* freq, Fvector2* range)
{
	if (!bPresent || 0==S._handle())return;
	verify_refsound		(S);
	ref_sound_data_ptr old = S._p;
	S._p				= xr_new<ref_sound_data>(*S._p);
	S._p->g_object		= O;
	i_play				(&S,flags&sm_Looped,delay);
	if (flags&sm_2D)	S._feedback()->switch_to_2D();
	if (pos)			S._feedback()->set_position	(*pos);
	if (freq)			S._feedback()->set_frequency(*freq);
	if (range)			S._feedback()->set_range   	((*range)[0],(*range)[1]);
	if (vol)			S._feedback()->set_volume   (*vol);
	S._p				= old;
}
void	CSoundRender_Core::play_at_pos			( ref_sound& S, CObject* O, const Fvector &pos, u32 flags, float delay)
{
	if (!bPresent || 0==S._handle())return;
	verify_refsound		(S);
	S._p->g_object		= O;
	if (S._feedback())	((CSoundRender_Emitter*)S._feedback())->rewind ();
	else				i_play					(&S,flags&sm_Looped,delay);
	S._feedback()->set_position					(pos);
	if (flags&sm_2D)	S._feedback()->switch_to_2D();
}
void	CSoundRender_Core::destroy	(ref_sound& S )
{
	if (S._feedback()){                   
		CSoundRender_Emitter* E		= (CSoundRender_Emitter*)S._feedback();
		E->stop						(FALSE);
	}
	S._p				= 0;
}                                                    

void CSoundRender_Core::_create_data( ref_sound_data& S, BOOL _3D, LPCSTR fName, int type)
{
	string_path			fn;
	strcpy				(fn,fName);
    if (strext(fn))		*strext(fn)	= 0;
	S.handle			= (CSound_source*)SoundRender->i_create_source(fn,_3D);
	S.g_type			= (type==st_SourceType)?S.handle->game_type():type;
	S.feedback			= 0; 
    S.g_object			= 0; 
    S.g_userdata		= 0;
}
void CSoundRender_Core::_destroy_data( ref_sound_data& S)
{
	if (S.feedback){                   
		CSoundRender_Emitter* E		= (CSoundRender_Emitter*)S.feedback;
		E->stop						(FALSE);
	}
	R_ASSERT						(0==S.feedback);
	SoundRender->i_destroy_source	((CSoundRender_Source*)S.handle);
	S.handle						= NULL;
}

CSoundRender_Environment*	CSoundRender_Core::get_environment			( const Fvector& P )
{
	static CSoundRender_Environment	identity;

	if (bUserEnvironment){
		return &s_user_environment;
	}else{
		if (geom_ENV){
			Fvector	dir				= {0,-1,0};
#ifdef _EDITOR
			ETOOLS::ray_options		(CDB::OPT_ONLYNEAREST);
			ETOOLS::ray_query		(geom_ENV,P,dir,1000.f);
			if (ETOOLS::r_count()){
				CDB::RESULT*		r	= ETOOLS::r_begin();
#else
			geom_DB.ray_options		(CDB::OPT_ONLYNEAREST);
			geom_DB.ray_query		(geom_ENV,P,dir,1000.f);
			if (geom_DB.r_count()){
				CDB::RESULT*		r	= geom_DB.r_begin();
#endif            
				CDB::TRI*			T	= geom_ENV->get_tris()+r->id;
				Fvector*			V	= geom_ENV->get_verts();
				Fvector tri_norm;
				tri_norm.mknormal		(V[T->verts[0]],V[T->verts[1]],V[T->verts[2]]);
				float	dot				= dir.dotproduct(tri_norm);
				if (dot<0){
					u16		id_front	= (u16)((T->dummy&0x0000ffff)>>0);		//	front face
					return	s_environment->Get(id_front);
				}else{
					u16		id_back		= (u16)((T->dummy&0xffff0000)>>16);	//	back face
					return	s_environment->Get(id_back);
				}
			}else{
				identity.set_identity	();
				return &identity;
			}
		}else{
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

void CSoundRender_Core::update_listener( const Fvector& P, const Fvector& D, const Fvector& N, float dt )
{
    clamp							(dt,EPS_S,1.f/10.f);
}

void	CSoundRender_Core::i_eax_listener_set	(CSound_environment* _E)
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

    u32 deferred				= bDeferredEAX?DSPROPERTY_EAXLISTENER_DEFERRED:0;
    
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ROOM, 					&ep.lRoom,					sizeof(LONG));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ROOMHF, 				&ep.lRoomHF,				sizeof(LONG));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ROOMROLLOFFFACTOR, 	&ep.flRoomRolloffFactor,	sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_DECAYTIME, 		  	&ep.flDecayTime,			sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_DECAYHFRATIO,			&ep.flDecayHFRatio,			sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REFLECTIONS, 			&ep.lReflections,			sizeof(LONG));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REFLECTIONSDELAY,    	&ep.flReflectionsDelay,		sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REVERB, 				&ep.lReverb,				sizeof(LONG));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_REVERBDELAY, 			&ep.flReverbDelay,			sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_ENVIRONMENTDIFFUSION,	&ep.flEnvironmentDiffusion,	sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF, 		&ep.flAirAbsorptionHF,		sizeof(float));
    i_eax_set(&DSPROPSETID_EAX_ListenerProperties, deferred | DSPROPERTY_EAXLISTENER_FLAGS, 				&ep.dwFlags,				sizeof(DWORD));
}

void	CSoundRender_Core::i_eax_listener_get	(CSound_environment* _E)
{
	VERIFY(bEAX);
    CSoundRender_Environment* E = static_cast<CSoundRender_Environment*>(_E);
    EAXLISTENERPROPERTIES 		ep;
	i_eax_get					(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ALLPARAMETERS, &ep, sizeof(EAXLISTENERPROPERTIES));
    E->Room						= (float)ep.lRoom					;
    E->RoomHF					= (float)ep.lRoomHF					;
    E->RoomRolloffFactor		= (float)ep.flRoomRolloffFactor		;
    E->DecayTime			   	= (float)ep.flDecayTime				;
    E->DecayHFRatio				= (float)ep.flDecayHFRatio			;
    E->Reflections				= (float)ep.lReflections			;
    E->ReflectionsDelay			= (float)ep.flReflectionsDelay		;
    E->Reverb					= (float)ep.lReverb					;
    E->ReverbDelay				= (float)ep.flReverbDelay			;
    E->EnvironmentSize			= (float)ep.flEnvironmentSize		;
    E->EnvironmentDiffusion		= (float)ep.flEnvironmentDiffusion	;
    E->AirAbsorptionHF			= (float)ep.flAirAbsorptionHF		;
}

void CSoundRender_Core::i_eax_commit_setting()
{
	// commit eax 
    if (bDeferredEAX)
    	i_eax_set(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_COMMITDEFERREDSETTINGS,NULL,0);
}

void CSoundRender_Core::object_relcase( CObject* obj )
{
	if (obj){
		for (u32 eit=0; eit<s_emitters.size(); eit++)
			if (s_emitters[eit]->owner_data && (obj==s_emitters[eit]->owner_data->g_object)) 
				s_emitters[eit]->owner_data->g_object=0;
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
void						CSoundRender_Core::refresh_sources()
{
	for (u32 eit=0; eit<s_emitters.size(); eit++)
    	s_emitters[eit]->stop(FALSE);
	for (u32 sit=0; sit<s_sources.size(); sit++){
    	CSoundRender_Source* s = s_sources[sit];
    	s->unload		();
		s->load			(*s->fname,s->_3D);
    }
}
void CSoundRender_Core::set_environment_size	(CSound_environment* src_env, CSound_environment** dst_env)
{
	if (bEAX){
		CSoundRender_Environment* SE 	= static_cast<CSoundRender_Environment*>(src_env); 
		CSoundRender_Environment* DE 	= static_cast<CSoundRender_Environment*>(*dst_env); 
		// set environment
		i_eax_set			    		(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_IMMEDIATE | DSPROPERTY_EAXLISTENER_ENVIRONMENTSIZE, &SE->EnvironmentSize, sizeof(SE->EnvironmentSize));
		i_eax_listener_set				(SE);
		i_eax_commit_setting			();
		i_eax_set			    		(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_IMMEDIATE | DSPROPERTY_EAXLISTENER_ENVIRONMENTSIZE, &DE->EnvironmentSize, sizeof(DE->EnvironmentSize));
		i_eax_listener_get				(DE);
	}
}
void CSoundRender_Core::set_environment	(u32 id, CSound_environment** dst_env)
{
	if (bEAX){
		CSoundRender_Environment* DE 	= static_cast<CSoundRender_Environment*>(*dst_env); 
		// set environment
		i_eax_set			    		(&DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_IMMEDIATE | DSPROPERTY_EAXLISTENER_ENVIRONMENTSIZE, &id, sizeof(id));
		i_eax_listener_get				(DE);
	}
}
#endif


