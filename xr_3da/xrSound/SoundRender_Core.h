#ifndef SoundRender_CoreH
#define SoundRender_CoreH
#pragma once

#include "SoundRender.h"
#include "SoundRender_Environment.h"

class CSoundRender_Core					: public CSound_manager_interface
{
private:
	struct SListener 
	{
		u32				dwSize;
		Fvector			vPosition;
		Fvector			vVelocity;
		Fvector			vOrientFront;
		Fvector			vOrientTop;
		float			fDistanceFactor;
		float			fRolloffFactor;
		float			fDopplerFactor;
	};
public:
	BOOL								bPresent;
	BOOL								bUserEnvironment;
	
	// DSound interface
	IDirectSound8*						pDevice;				// The device itself
	IDirectSoundBuffer*					pBuffer;				// The primary buffer (mixer destination)
	IDirectSound3DListener8*			pListener;
	LPKSPROPERTYSET						pExtensions;
	SListener							Listener;
	CTimer								Timer;
	sound_event*						Handler;
private:
	// Collider
	CDB::COLLIDER						geom_DB;
	CDB::MODEL*							geom_MODEL;
	CDB::MODEL*							geom_ENV;

	// Containers
	vector<CSoundRender_Source*>		s_sources;
	vector<CSoundRender_Emitter*>		s_emitters;
	vector<CSoundRender_Target*>		s_targets;
	vector<CSoundRender_Target*>		s_targets_defer;
	u32									s_targets_pu;			// parameters update
	SoundEnvironment_LIB*				s_environment;
	CSoundRender_Environment			s_user_environment;
public:
	CSoundRender_Core					();
	~CSoundRender_Core					();

	// General
	virtual void						_initialize				( u32 window );
	virtual void						_destroy				( );
	virtual void						_restart				( );

	// Sound interface
	virtual void						create					( sound& S, BOOL _3D,	LPCSTR fName,	int		type=0);
	virtual void						destroy					( sound& S);
	virtual void						play					( sound& S, CObject* O,								BOOL bLoop=false);
	virtual void						play_unlimited			( sound& S, CObject* O,								BOOL bLoop=false);
	virtual void						play_at_pos				( sound& S, CObject* O,		const Fvector &pos,		BOOL bLoop=false);
	virtual void						play_at_pos_unlimited	( sound& S, CObject* O,		const Fvector &pos,		BOOL bLoop=false);
	virtual void						set_geometry_env		( IReader* I );
	virtual void						set_geometry_occ		( CDB::MODEL* M );
	virtual void						set_handler				( sound_event* E );

	virtual void						update					( const Fvector& P, const Fvector& D, const Fvector& N, float dt );
	virtual u32							stat_render				( );
	virtual u32							stat_simulate			( );

#ifdef _EDITOR
	virtual SoundEnvironment_LIB*		get_env_library			()																{ return s_environment; }
	virtual void						refresh_env_library		();
	virtual void						set_user_env			( CSound_environment* E);
#endif
public:
	CSoundRender_Source*				i_create_source			( LPCSTR name, BOOL _3D		);
	void								i_destroy_source		( CSoundRender_Source*  S	);
	CSoundRender_Emitter*				i_play					( sound* S, BOOL _loop		);
	void								i_start					( CSoundRender_Emitter* E	);
	void								i_stop					( CSoundRender_Emitter* E	);
	void								i_rewind				( CSoundRender_Emitter* E	);
	BOOL								i_allow_play			( CSoundRender_Emitter* E	);

	BOOL								get_occlusion			( Fvector& P, float R, Fvector* occ );
	CSoundRender_Environment*			get_environment			( Fvector& P );

	void								env_load				();
	void								env_unload				();
	void								env_apply				();
};

extern CSoundRender_Core	SoundRender;
#endif