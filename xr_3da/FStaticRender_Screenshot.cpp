#include "stdafx.h"
#include "render.h"
#include "fs.h"
#include "tga.h"

#define RGBA_GETALPHA(rgb)      DWORD((rgb) >> 24)
#define RGBA_GETRED(rgb)        DWORD(((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      DWORD(((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       DWORD((rgb) & 0xff)

IC DWORD convert(float c)
{
	DWORD C=iFloor(c);
	if (C>255) C=255;
	return C;
}

void CRender::Screenshot		()
{
	if (!Device.bReady) return;
	if ((psDeviceFlags&rsFullscreen) == 0) {
		Log("! Can't capture screen while in windowed mode...");
		return;
	}
	
	// Create temp-surface
	IDirect3DSurface8*	pFB;
	R_CHK(HW.pDevice->CreateImageSurface(
		Device.dwWidth,Device.dwHeight,D3DFMT_A8R8G8B8,&pFB));
	R_CHK(HW.pDevice->GetFrontBuffer(pFB));
	
	D3DLOCKED_RECT	D;
	R_CHK(pFB->LockRect(&D,0,D3DLOCK_NOSYSLOCK));
	
	// Image processing
	DWORD* pPixel	= (DWORD*)D.pBits;
	DWORD* pEnd		= pPixel+(Device.dwWidth*Device.dwHeight);
	
	D3DGAMMARAMP G;
	Device.Gamma.GenLUT(G);
	for (int i=0; i<256; i++) {
		G.red	[i]	/= 256;
		G.green	[i]	/= 256;
		G.blue	[i]	/= 256;
	}
	
	for (;pPixel!=pEnd; pPixel++)
	{
		DWORD p = *pPixel;
		*pPixel = D3DCOLOR_XRGB(
			G.red	[RGBA_GETRED(p)],
			G.green	[RGBA_GETGREEN(p)],
			G.blue	[RGBA_GETBLUE(p)]
			);
	}
	
    TGAdesc			p;
    p.format		= IMG_24B;
    p.scanlenght	= D.Pitch;
    p.width			= Device.dwWidth;
    p.height		= Device.dwHeight;
    p.data			= D.pBits;
	
    p.maketga		("sshot");
	
	R_CHK(pFB->UnlockRect());
	pFB->Release	();
}
