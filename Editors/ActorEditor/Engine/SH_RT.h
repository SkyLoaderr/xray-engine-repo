#ifndef SH_RT_H
#define SH_RT_H
#pragma once

class	ENGINE_API	CTexture;

class	ENGINE_API	CRT
{
public:
	IDirect3DTexture9*	pSurface;
	IDirect3DSurface9*	pRT;
	CTexture*			pTexture;

	u32					dwReference;
	u32					dwWidth;
	u32					dwHeight;
	D3DFORMAT			fmt;

	CRT					()
	{
		Memory.mem_fill	(this,0,sizeof(CRT));
	}

	void				Create	(LPCSTR Name, u32 w, u32 h, D3DFORMAT f);
	void				Destroy	();
	IC BOOL				Valid	()	{ return 0!=pTexture; }
};

#endif // SH_RT_H
