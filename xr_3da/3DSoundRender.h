// 3DSoundRender.h: interface for the C3DSoundRender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DSOUNDRENDER_H__1D9A4469_E055_453C_8E18_8C5A23820A1A__INCLUDED_)
#define AFX_3DSOUNDRENDER_H__1D9A4469_E055_453C_8E18_8C5A23820A1A__INCLUDED_
#pragma once

// refs
class ENGINE_API C3DSound;
class ENGINE_API CInifile;

// CLASS
class ENGINE_API C3DSoundRender
{
	LPDIRECTSOUND3DLISTENER		pListener;
	F3dlistener					Listener;
	LPKSPROPERTYSET				pExtensions;

	vector <vector<C3DSound*> >	sounds;
	vector <int>				refcounts;

	vector <FIntersectionQuad>	clip_planes;

	C3DSound*					GetFreeSound	(int hSound);
	int							FindByName		(LPCSTR name, BOOL bFreq);
	int							FindEmptySlot	();
	int							Append			(C3DSound *p);
public:
	void						LoadClipPlanes		(CInifile *pIni, LPCSTR section);
	void						UnloadClipPlanes	( ) { clip_planes.clear(); }
	void						RenderClipPlanes	( );

	int							CreateSound	(LPCSTR name, BOOL bCtrlFreq=FALSE, BOOL bNotClip=FALSE );
	int							CreateSound	(CInifile *pIni, LPCSTR section);
	void						DeleteSound	(int& hSound);
	void						Play		(int  hSound, C3DSound** P, BOOL bLoop=false, int iLoopCnt=0);

	void						Reload		();

	void						OnMove		(void);

	C3DSoundRender();
	~C3DSoundRender();
};

#endif // !defined(AFX_3DSOUNDRENDER_H__1D9A4469_E055_453C_8E18_8C5A23820A1A__INCLUDED_)
