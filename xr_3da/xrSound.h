#pragma once

#include "sound.h"

// refs
class CCDA;
class CSoundRender;
class CMusicStream;
class CSound;

// t-defs
class CSoundManager :
	public CSound_manager_interface
{
public:	
	BOOL							bPresent;
	
	// Saves
	float							fMasterVolume;
	float							fMusicVolume;
	u32								dwFreq;
	u32								dwModel;
	
	// Children
	CCDA*							pCDA;
	CSoundRender*					pSoundRender;
	CMusicStream*					pMusicStreams;

	// Collider
	CDB::COLLIDER					DB;

	// DirectSound interface
    IDirectSound*					pDevice;				// The device itself
	IDirectSoundBuffer*				pBuffer;				// The primary buffer (mixer destination)

	// Geometry
	CDB::MODEL*						pGeometry;
private:
	BOOL							CreatePrimaryBuffer		( );
	void							InitDS3D				( );
	void							SetVMusic				( );
public:
	CSoundManager					( );
	~CSoundManager					( );

	// General
	virtual void					Initialize				( )																						;
	virtual void					Destroy					( )																						;
	virtual void					Restart					( )																						;

	// Sound interface
	virtual void					Create					( sound& S, BOOL _3D,	LPCSTR fName,	BOOL bCtrlFreq=FALSE,	int		type=0)			;
	virtual void					Play					( sound& S, CObject* O,								BOOL bLoop=false,	int	iLoopCnt=0)	;
	virtual void					Play_Unlimited			( sound& S, CObject* O,								BOOL bLoop=false,	int	iLoopCnt=0)	;
	virtual void					PlayAtPos				( sound& S, CObject* O,		const Fvector &pos,		BOOL bLoop=false,	int iLoopCnt=0)	;
	virtual void					PlayAtPos_Unlimited		( sound& S, CObject* O,		const Fvector &pos,		BOOL bLoop=false,	int iLoopCnt=0)	;
	virtual void					Delete					( sound& S)																				;
	virtual void					SetGeometry				( CDB::MODEL* M )																		;
	virtual CDB::MODEL* 			GetGeometry				( )																						;
	virtual BOOL					IsOccluded				( Fvector& P, float R, Fvector* occ )													;

	// Stream interface
	virtual CSound_stream_interface*CreateStream			( LPCSTR fName )																		;
	virtual void					DeleteStream			( CSound_stream_interface* pSnd )														;

	virtual void					OnFrame					( )																						;
};

extern CSoundManager				Sound_Implementation;
