#ifndef SH_RT_H
#define SH_RT_H

class	ENGINE_API	CRT
{
public:
	IDirect3DTexture8*	pSurface;
	IDirect3DSurface8*	pRT;
	CTexture*			pTexture;

	CRT					(){	ZeroMemory(this,sizeof(CRT));}

	
};

#endif // SH_RT_H