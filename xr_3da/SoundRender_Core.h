#pragma once

#include "SoundRender.h"

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
	
	// DSound interface
	IDirectSound8*						pDevice;				// The device itself
	IDirectSoundBuffer*					pBuffer;				// The primary buffer (mixer destination)
	IDirectSound3DListener8*			pListener;
	LPKSPROPERTYSET						pExtensions;
	SListener							Listener;
private:
	// Collider
	CDB::COLLIDER						geom_DB;
	CDB::MODEL*							geom_MODEL;

	// Containers
	vector<CSoundRender_Source*>		s_sources;
	vector<CSoundRender_Emitter*>		s_emitters;
	vector<CSoundRender_Target*>		s_targets;
	vector<CSoundRender_Target*>		s_targets_defer;
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
	virtual void						set_geometry			( CDB::MODEL* M );
	virtual CDB::MODEL* 				get_geometry			( );

	virtual void						OnFrame					( );

public:
	void								i_start					(CSoundRender_Emitter* E);
	void								i_stop					(CSoundRender_Emitter* E);
	void								i_rewind				(CSoundRender_Emitter* E);

	BOOL								get_occlusion			( Fvector& P, float R, Fvector* occ );
	CSoundRender_Environment*			get_environment			( Fvector& P );
};

extern CSoundRender_Core	SoundRender;
