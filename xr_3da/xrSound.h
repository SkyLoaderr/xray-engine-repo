#pragma once

#include "sound.h"

// refs
class CCDA;
class CMusicStream;
class CSound;

// t-defs
class CSoundManager					:	public CSound_manager_interface
{
public:	
	
	// Children
	CCDA*							pCDA;
	CSoundRender*					pSoundRender;
	CMusicStream*					pMusicStreams;
private:
	BOOL							CreatePrimaryBuffer		( );
	void							InitDS3D				( );
	void							SetVMusic				( );
public:
	CSoundManager					( );
	~CSoundManager					( );

	// General
	virtual void					Initialize				( )																		;
	virtual void					Destroy					( )																		;
	virtual void					Restart					( )																		;

	// Sound interface
	virtual void					Create					( sound& S, BOOL _3D,	LPCSTR fName,	int		type=0)					;
	virtual void					Play					( sound& S, CObject* O,								BOOL bLoop=false)	;
	virtual void					Play_Unlimited			( sound& S, CObject* O,								BOOL bLoop=false)	;
	virtual void					PlayAtPos				( sound& S, CObject* O,		const Fvector &pos,		BOOL bLoop=false)	;
	virtual void					PlayAtPos_Unlimited		( sound& S, CObject* O,		const Fvector &pos,		BOOL bLoop=false)	;
	virtual void					Delete					( sound& S)																;
	virtual void					SetGeometry				( CDB::MODEL* M )														;
	virtual CDB::MODEL* 			GetGeometry				( )																		;
	virtual BOOL					IsOccluded				( Fvector& P, float R, Fvector* occ )									;

	// Stream interface
	virtual CSound_stream_interface*CreateStream			( LPCSTR fName )														;
	virtual void					DeleteStream			( CSound_stream_interface* pSnd )										;

	virtual void					OnFrame					( )																		;
};

extern CSoundManager				Sound_Implementation;
