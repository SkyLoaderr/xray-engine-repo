#ifndef __XR_SNDMAN_H__
#define __XR_SNDMAN_H__

#include "xr_snd_defs.h"

class ENGINE_API CSoundManager : public pureFrame
{
	friend class			CSoundStream;
	friend class			C3DSoundRender;
	friend class			C3DSound;
	friend class			C2DSoundRender;
	friend class			C2DSound;
private:	
	BOOL					bPresent;
	
	// saves
	float					fMasterVolume;
	float					fMusicVolume;
	DWORD					dwFreq;
	DWORD					dwModel;
	
	// Chields
	CCDA*					pCDA;
	C3DSoundRender*			pSoundRender;
	C2DSoundRender*			p2DSounds;
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
//	void					SetFreq					( );
//	void					SetModel				( );
	
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
	
	// 2D interface
	int						Create2D				( CInifile* ini, LPCSTR section );
	int						Create2D				( LPCSTR fName	);
	void					Delete2D				( int &h );
	C2DSound*				Play2D					( int handle, BOOL bLoop=false, int iLoopCnt=0);
	
	// 3D interface
	void					Create3D				( sound3D& S, CInifile* ini, LPCSTR section );
	void					Create3D				( sound3D& S, LPCSTR fName, BOOL bCtrlFreq=FALSE );
	void					Play3D					( sound3D& S, CObject* O, BOOL bLoop=false, int iLoopCnt=0);
	void					Play3D_Unlimited		( sound3D& S, CObject* O, BOOL bLoop=false, int iLoopCnt=0);
	void					Play3DAtPos				( sound3D& S, CObject* O, const Fvector &pos, BOOL bLoop=false, int iLoopCnt=0);
	void					Play3DAtPos_Unlimited	( sound3D& S, CObject* O, const Fvector &pos, BOOL bLoop=false, int iLoopCnt=0);
	void					Delete3D				( sound3D& S);
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
