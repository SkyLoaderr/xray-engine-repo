// 3DSoundRender.h: interface for the C3DSoundRender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSOUNDRENDER_H__1D9A4469_E055_453C_8E18_8C5A23820A1A__INCLUDED_)
#define AFX_3DSOUNDRENDER_H__1D9A4469_E055_453C_8E18_8C5A23820A1A__INCLUDED_
#pragma once

#include "xr_snd_defs.h"

// refs
class ENGINE_API C3DSound;
class ENGINE_API CInifile;

// CLASS
class ENGINE_API C3DSoundRender
{
private:
	struct SListener 
	{
		DWORD			dwSize;
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
	vector <vector<C3DSound*> >	sounds;
	vector <int>				refcounts;
private:
	C3DSound*					GetFreeSound		(int hSound);
	int							FindByName			(LPCSTR name, BOOL bFreq);
	int							FindEmptySlot		();
	int							Append				(C3DSound *p);
public:
	int							CreateSound			(LPCSTR name, BOOL bCtrlFreq=FALSE, BOOL bNotClip=FALSE );
	int							CreateSound			(CInifile *pIni, LPCSTR section);
	void						DeleteSound			(int& hSound);
	void						Play				(int  hSound, sound3D* P, BOOL bLoop=false, int iLoopCnt=0);

	void						Reload				();

	void						OnMove				(void);

	C3DSoundRender();
	~C3DSoundRender();
};

#endif // !defined(AFX_3DSOUNDRENDER_H__1D9A4469_E055_453C_8E18_8C5A23820A1A__INCLUDED_)
