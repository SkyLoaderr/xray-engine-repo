// 3DSound.h: interface for the CSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSOUND_H__4CEA9FC5_4F0D_441A_AED2_8027C1395B54__INCLUDED_)
#define AFX_3DSOUND_H__4CEA9FC5_4F0D_441A_AED2_8027C1395B54__INCLUDED_
#pragma once

#include "xrSound.h"

struct 	sound_defer
{
	LPVOID 							P;
	u32								F;

	sound_defer(LPVOID _P, u32 _F) : P(_P), F(_F) {};
};


class ENGINE_API CSound :
	public CSound_interface
{
public:
	LPSTR				fName;

	enum State
	{
		stStopped		= 0,
		stPlaying,
		stPlayingLooped,
		stSimulating,
		stSimulatingLooped,
		
		stFORCEDWORD	= u32(-1)
	};
	
	struct Control 
	{
	public:
		u32				dwSize;
		Fvector			vPosition;
		Fvector			vVelocity;
		u32				dwInsideConeAngle;
		u32				dwOutsideConeAngle;
		Fvector			vConeOrientation;
		int				lConeOutsideVolume;
		float			flMinDistance;
		float			flMaxDistance;
		u32				dwMode;

		IC	DS3DBUFFER *d3d(void) { return (DS3DBUFFER *)this; };

		IC	void		init	( )
		{
			dwSize				= sizeof(DS3DBUFFER);
			vPosition.set		( 0.0f, 0.0f, 0.0f );
			vVelocity.set		( 0.0f, 0.0f, 0.0f );
			vConeOrientation.set( 0.0f, 0.0f, 1.0f );
			flMinDistance		= DS3D_DEFAULTMINDISTANCE;
			flMaxDistance		= DS3D_DEFAULTMAXDISTANCE;
			dwInsideConeAngle	= DS3D_DEFAULTCONEANGLE;
			dwOutsideConeAngle	= DS3D_DEFAULTCONEANGLE;
			lConeOutsideVolume	= DS3D_DEFAULTCONEOUTSIDEVOLUME;
			dwMode				= DS3DMODE_NORMAL;
		}
		IC	void		set		( const Control& b )
		{
			dwSize				= b.dwSize;
			vPosition.set		( b.vPosition );
			vVelocity.set		( b.vVelocity);
			vConeOrientation.set( b.vConeOrientation );
			flMinDistance		= b.flMinDistance;
			flMaxDistance		= b.flMaxDistance;
			dwInsideConeAngle	= b.dwInsideConeAngle;
			dwOutsideConeAngle	= b.dwOutsideConeAngle;
			lConeOutsideVolume	= b.lConeOutsideVolume;
			dwMode				= b.dwMode;
		}
	};

public:
	sound*					owner;

	float 					fVolume;				// USER
	float					fBaseVolume;			// CLIPPING
	float 					fRealVolume;
	Fvector					occluder[3];

	u32						dwTimeTotal;			// всего
	u32						dwTimeStarted;			// time of "Start"
	u32						dwTimeToStop;			// time to "Stop"
	u32						dwTimeToPropagade;

	u32						dwState;

    IDirectSoundBuffer*		pBuffer;
    IDirectSound3DBuffer8*	pBuffer3D;
	LPKSPROPERTYSET			pExtensions;
	u32						dwBytesPerMS;
	Control					ps;						// property sets for the 3d-buffer
	BOOL					bNeedUpdate;

	BOOL					bMustPlay;
	BOOL					bMustLoop;
	int						iLoopCount;

	BOOL					_3D;
	BOOL					_Freq;
	u32						dwFreq;
	u32						dwFreqBase;

	u32						dwLastTimeActive;
public:
	IDirectSoundBuffer*		LoadWaveAs3D			(LPCSTR name,	BOOL bCtrlFreq);
	IDirectSoundBuffer*		LoadWaveAs2D			(LPCSTR name,	BOOL bCtrlFreq);
	void					Load					(LPCSTR,		BOOL bCtrlFreq=FALSE);		// wav-file
	void					Load					(const CSound *);							// clone

	void					PropagadeEvent			();
	void					internalStopOrComplete	();
public:
	BOOL					isPlaying				(void)	{ return (dwState!=stStopped) || bMustPlay; }

	virtual void			Play					(sound* P, BOOL bLoop=false, int lcnt=0);
	virtual void			Rewind					();
	virtual void			Stop					(void);
	virtual void			SetPosition				(const Fvector &pos);
	virtual void			SetFrequency			(u32 freq);
	virtual void			SetFrequencyScale		(float scale);
	virtual void			SetMinMax				(float min, float max);
	virtual void			SetVolume				(float vol)			{ fVolume = vol; bNeedUpdate = true; }

	void					OnMove					(vector<sound_defer>& plist);
	void					Update_Params			(void);
	void					Update_Occlusion		(void);
	BOOL					Update_Volume			(void);			// returns TRUE if "audible"

	CSound					(BOOL b_3D);
	~CSound					();
};

#endif // !defined(AFX_3DSOUND_H__4CEA9FC5_4F0D_441A_AED2_8027C1395B54__INCLUDED_)
