#ifndef __XR_SNDMAN_H__
#define __XR_SNDMAN_H__

#include "xr_snd_defs.h"

class ENGINE_API CSoundManager : public pureFrame
{
	friend class			CSoundStream;
	friend class			CSoundRender;
	friend class			CSound;
private:	
	BOOL					bPresent;
	
	// saves
	float					fMasterVolume;
	float					fMusicVolume;
	DWORD					dwFreq;
	DWORD					dwModel;
	
	// Chields
	CCDA*					pCDA;
	CSoundRender*			pSoundRender;
	CMusicStream*			pMusicStreams;
	
	// DirectSound interface
    LPDIRECTSOUND		    pDevice;				// The device itself
    LPDIRECTSOUNDBUFFER		pBuffer;				// The primary buffer (mixer destination)

	// Geometry
	CDB::MODEL*				pGeometry;
private:
	BOOL					CreatePrimaryBuffer		( );
	void					Initialize				( );
	
	void					SetVMusic				( );
public:
	CSoundManager			( BOOL bCDA = false );
	~CSoundManager			( );
	
	// general function
	BOOL					isPresent				( )				{ return bPresent; }
	void					GetDeviceInfo			( );
	void					Restart					( );
	
	// CD interface
	void					OpenCDTrack				( CInifile* ini, LPCSTR section );
	void					PlayCDTrack				( );
	void					StopCDTrack				( );
	
	// Sound interface
	void					Create					( sound& S, BOOL _3D=TRUE,	CInifile* ini,	LPCSTR section,			int		type=0);
	void					Create					( sound& S, BOOL _3D=TRUE,	LPCSTR fName,	BOOL bCtrlFreq=FALSE,	int		type=0);
	void					Play					( sound& S, CObject* O,								BOOL bLoop=false,	int	iLoopCnt=0);
	void					Play_Unlimited			( sound& S, CObject* O,								BOOL bLoop=false,	int	iLoopCnt=0);
	void					PlayAtPos				( sound& S, CObject* O,		const Fvector &pos,		BOOL bLoop=false,	int iLoopCnt=0);
	void					PlayAtPos_Unlimited		( sound& S, CObject* O,		const Fvector &pos,		BOOL bLoop=false,	int iLoopCnt=0);
	void					Delete					( sound& S);
	void					SetGeometry				( CDB::MODEL* M )	{ pGeometry=M; }
	CDB::MODEL* 			GetGeometry				( )					{ return pGeometry;	}
	BOOL					IsOccluded				( Fvector& P, float R, soundOccluder& occ );
	
	// Stream interface
	CSoundStream*			CreateStream			( CInifile* ini, LPCSTR section );
	CSoundStream*			CreateStream			( LPCSTR fName );
	void					DeleteStream			( CSoundStream* pSnd );
	
	virtual void			OnFrame					( );
};

extern ENGINE_API CSoundManager* pSounds;

#endif //__XR_SNDMAN_H__
