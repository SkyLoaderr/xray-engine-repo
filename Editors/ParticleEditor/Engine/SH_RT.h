#ifndef SH_RT_H
#define SH_RT_H
#pragma once

class	ENGINE_API	CTexture;

class	ENGINE_API	CRT
{
public:
	IDirect3DTexture8*	pSurface;
	IDirect3DSurface8*	pRT;
	CTexture*			pTexture;
	
	u32				dwReference;
	u32				dwWidth;
	u32				dwHeight;

	CRT					(){	ZeroMemory(this,sizeof(CRT));}

	void				Create	(LPCSTR Name, u32 w, u32 h);
	void				Destroy	();
	IC BOOL				Valid	()	{ return 0!=pTexture; }
};

#endif // SH_RT_H