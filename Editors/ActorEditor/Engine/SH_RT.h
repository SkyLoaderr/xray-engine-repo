#ifndef SH_RT_H
#define SH_RT_H
#pragma once

class	ENGINE_API	CTexture;

class	ENGINE_API	CRT		:	public xr_resource_named	{
public:
	IDirect3DTexture9*	pSurface;
	IDirect3DSurface9*	pRT;
	ref_texture			pTexture;

	u32					dwWidth;
	u32					dwHeight;
	D3DFORMAT			fmt;

	CRT					();
	~CRT				();

	void				Create	(LPCSTR Name, u32 w, u32 h, D3DFORMAT f);
	void				Destroy	();
	IC BOOL				Valid	()	{ return 0!=pTexture; }
};
typedef	resptr_core<CRT,resptr_base<CRT> >					ref_rt;

class	ENGINE_API	CRTC	:	public xr_resource_named	{
public:
	IDirect3DCubeTexture9*	pSurface;
	IDirect3DSurface9*		pRT[6];
	ref_texture				pTexture;

	u32						dwSize;
	D3DFORMAT				fmt;

	CRTC					();
	~CRTC					();

	void				Create	(LPCSTR name, u32 size, D3DFORMAT f);
	void				Destroy	();
	IC BOOL				Valid	()	{ return 0!=pTexture; }
};
typedef	resptr_core<CRTC,resptr_base<CRTC> >				ref_rtc;

#endif // SH_RT_H
