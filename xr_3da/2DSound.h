// 2DSound.h: interface for the C2DSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_2DSOUND_H__C81EC45C_B4F1_43C1_9912_AAA3989E301F__INCLUDED_)
#define AFX_2DSOUND_H__C81EC45C_B4F1_43C1_9912_AAA3989E301F__INCLUDED_

#pragma once

class ENGINE_API CInifile;

class ENGINE_API C2DSound
{
	friend class C2DSoundRender;
public:
	char*					fName;
private:
	float 					fVolume;
	float					fBaseVolume;
	float 					fRealVolume;
	float					fTimeTotal;			// всего
	float					fTimeRested;		// осталось

	DWORD					dwStatus;

    LPDIRECTSOUNDBUFFER     pBuffer;
	BOOL					bNeedUpdate;

	BOOL					bMustPlay;
	BOOL					bMustLoop;
	int						iLoopCountRested;
private:
	LPDIRECTSOUNDBUFFER		LoadWaveAs2D	(const char *name);
	void					Load			(CInifile *, const char *sect);	// wav+params
	void					Load			(const char *);					// wav-file
	void					Load			(C2DSound *);					// clone
public:
	BOOL					isPlaying		(void)				{ return (dwStatus&DSBSTATUS_PLAYING)||bMustPlay; }

	void					Play			(BOOL bLoop=false, int lcnt=0);
	void					Stop			(void);
	void					SetVolume		(float vol)			{ fVolume = vol; bNeedUpdate = true; }

	void					OnMove			(void);
	void					Update			(void);

	C2DSound();
	~C2DSound();
};

#endif // !defined(AFX_2DSOUND_H__C81EC45C_B4F1_43C1_9912_AAA3989E301F__INCLUDED_)
