// 3DSoundRender.h: interface for the CSoundRender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSOUNDRENDER_H__1D9A4469_E055_453C_8E18_8C5A23820A1A__INCLUDED_)
#define AFX_3DSOUNDRENDER_H__1D9A4469_E055_453C_8E18_8C5A23820A1A__INCLUDED_
#pragma once

#include "xr_snd_defs.h"

// refs
class ENGINE_API CSound;
class ENGINE_API CInifile;

// CLASS
class ENGINE_API CSoundRender
{
private:
	struct SListener 
	{
		u32			dwSize;
		Fvector			vPosition;
		Fvector			vVelocity;
		Fvector			vOrientFront;
		Fvector			vOrientTop;
		float			fDistanceFactor;
		float			fRolloffFactor;
		float			fDopplerFactor;
	};
private:
	// DirectSound interface
	LPDIRECTSOUND3DLISTENER		pListener;
	LPKSPROPERTYSET				pExtensions;
	SListener					Listener;
	
	// Containers
	vector <vector<CSound*> >	sounds;
	vector <int>				refcounts;
	vector <sound_defer>		defer;
private:
	CSound*						GetFreeSound		(int hSound);
	int							FindByName			(LPCSTR name, BOOL _3D, BOOL _Freq);
	int							FindEmptySlot		();
	int							Append				(CSound *p);
public:
	int							CreateSound			(LPCSTR name, BOOL _3D, BOOL _Freq=FALSE, BOOL bNotClip=FALSE );
	int							CreateSound			(CInifile *pIni, LPCSTR section);
	void						DeleteSound			(int& hSound);
	void						Play				(int  hSound, sound* P, BOOL bLoop=false, int iLoopCnt=0);

	void						Reload				();

	void						OnMove				(void);

	CSoundRender();
	~CSoundRender();
};

#endif // !defined(AFX_3DSOUNDRENDER_H__1D9A4469_E055_453C_8E18_8C5A23820A1A__INCLUDED_)
