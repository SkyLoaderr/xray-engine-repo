// 3DSound.h: interface for the CSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSOUND_H__4CEA9FC5_4F0D_441A_AED2_8027C1395B54__INCLUDED_)
#define AFX_3DSOUND_H__4CEA9FC5_4F0D_441A_AED2_8027C1395B54__INCLUDED_
#pragma once

#include "xr_snd_defs.h"

class ENGINE_API CInifile;

class ENGINE_API CSound
{
	friend class CSoundRender;
public:
	char*					fName;

	enum State
	{
		stStopped		= 0,
		stPlaying,
		stPlayingLooped,
		stSimulating,
		stSimulatingLooped,
		
		stFORCEDWORD	= DWORD(-1)
	};
private:
	sound*					owner;

	float 					fVolume;				// USER
	float					fBaseVolume;			// CLIPPING
	float 					fRealVolume;
	soundOccluder			occluder;

	DWORD					dwTimeTotal;			// всего
	DWORD					dwTimeStarted;			// time of "Start"
	DWORD					dwTimeToStop;			// time to "Stop"
	DWORD					dwTimeToPropagade;

	DWORD					dwState;

    LPDIRECTSOUNDBUFFER     pBuffer;
    LPDIRECTSOUND3DBUFFER   pBuffer3D;
	LPKSPROPERTYSET			pExtensions;
	DWORD					dwBytesPerMS;
	F3dbuffer				ps;						// property sets for the 3d-buffer
	BOOL					bNeedUpdate;

	BOOL					bMustPlay;
	BOOL					bMustLoop;
	int						iLoopCount;

	BOOL					_3D;
	BOOL					_Freq;
	DWORD					dwFreq;

	DWORD					dwLastTimeActive;
private:
	LPDIRECTSOUNDBUFFER		LoadWaveAs3D			(LPCSTR name,	BOOL bCtrlFreq);
	LPDIRECTSOUNDBUFFER		LoadWaveAs2D			(LPCSTR name,	BOOL bCtrlFreq);
	void					Load					(LPCSTR,		BOOL bCtrlFreq=FALSE);		// wav-file
	void					Load					(CInifile *,	LPCSTR sect);				// wav+params
	void					Load					(const CSound *);							// clone

	void					PropagadeEvent			();
	void					internalStopOrComplete	();
public:
	BOOL					isPlaying				(void)	{ return (dwState!=stStopped) || bMustPlay; }

	void					Play					(sound* P, BOOL bLoop=false, int lcnt=0);
	void					Rewind					();
	void					Stop					(void);
	void					SetPosition				(const Fvector &pos);
	void					SetFrequency			(DWORD freq);
	void					SetMinMax				(float min, float max);
	void					SetVolume				(float vol)			{ fVolume = vol; bNeedUpdate = true; }

	void					OnMove					(void);
	void					Update_Params			(void);
	void					Update_Occlusion		(void);
	BOOL					Update_Volume			(void);			// returns TRUE if "audible"

	CSound					(BOOL b_3D);
	~CSound					();
};

#endif // !defined(AFX_3DSOUND_H__4CEA9FC5_4F0D_441A_AED2_8027C1395B54__INCLUDED_)
