// 3DSound.h: interface for the C3DSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSOUND_H__4CEA9FC5_4F0D_441A_AED2_8027C1395B54__INCLUDED_)
#define AFX_3DSOUND_H__4CEA9FC5_4F0D_441A_AED2_8027C1395B54__INCLUDED_
#pragma once

class ENGINE_API CInifile;

class ENGINE_API C3DSound
{
	friend class C3DSoundRender;
public:
	char*					fName;
private:
	C3DSound**				owner;

	float 					fVolume;
	float					fBaseVolume;
	float 					fRealVolume;
	float					fTimeTotal;			// всего
	float					fTimeRested;		// осталось

	DWORD					dwStatus;

    LPDIRECTSOUNDBUFFER     pBuffer;
    LPDIRECTSOUND3DBUFFER   pBuffer3D;
	LPKSPROPERTYSET			pExtensions;
	F3dbuffer				ps;			// property sets for the 3d-buffer
	BOOL					bNeedUpdate;

	BOOL					bMustPlay;
	BOOL					bMustLoop;
	BOOL					bCtrlFreq;
	DWORD					dwFreq;
	int						iLoopCountRested;

	DWORD					dwLastTimeActive;
private:
	LPDIRECTSOUNDBUFFER		LoadWaveAs3D			(LPCSTR name, BOOL bCtrlFreq);
	void					Load					(CInifile *, LPCSTR sect);			// wav+params
	void					Load					(LPCSTR, BOOL bCtrlFreq=FALSE);		// wav-file
	void					Load					(const C3DSound *);					// clone

	void					internalStopOrComplete	();
public:
	BOOL					isPlaying				(void)	{ return (dwStatus&DSBSTATUS_PLAYING)||bMustPlay; }

	void					Play					(C3DSound** P, BOOL bLoop=false, int lcnt=0);
	void					Rewind					();
	void					Stop					(void);
	void					SetPosition				(const Fvector &pos);
	void					SetFrequency			(DWORD freq);
	void					SetMinMax				(float min, float max);
	void					SetVolume				(float vol)			{ fVolume = vol; bNeedUpdate = true; }

	float					GetPercentageRested	()	{ return fTimeRested/fTimeTotal; }

	void					OnMove					(void);
	void					Update					(void);

	C3DSound();
	~C3DSound();
};

#endif // !defined(AFX_3DSOUND_H__4CEA9FC5_4F0D_441A_AED2_8027C1395B54__INCLUDED_)
