#include "stdafx.h"
#include "fstaticrender.h"
#include "fs.h"
#include "tga.h"
#include "xr_creator.h"

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
IC void MouseRayFromPoint	( Fvector& direction, int x, int y, Fmatrix& m_CamMat )
{
	int halfwidth  = Device.dwWidth/2;
	int halfheight = Device.dwHeight/2;

	Ipoint point2;
	point2.set(x-halfwidth, halfheight-y);

	float size_y = VIEWPORT_NEAR * tanf( deg2rad(60.f) * 0.5f );
	float size_x = size_y / (Device.fHeight_2/Device.fWidth_2);

	float r_pt	= float(point2.x) * size_x / (float) halfwidth;
	float u_pt	= float(point2.y) * size_y / (float) halfheight;

	direction.mul( m_CamMat.k, VIEWPORT_NEAR );
	direction.mad( direction, m_CamMat.j, u_pt );
	direction.mad( direction, m_CamMat.i, r_pt );
	direction.normalize();
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

	// 
	Fmatrix	mInv;	mInv.invert	(Device.mView);
	float	fFAR	= 100.f;
	XRC.ray_options	(CDB::OPT_CULL|CDB::OPT_ONLYNEAREST);
	for (u32 y=0; y<Device.dwHeight; y++)
	{
		for (u32 x=0; x<Device.dwWidth; x++)
		{
			DWORD* p			= ((DWORD*)D.pBits) + y*Device.dwWidth+x;

			Fvector D;
			MouseRayFromPoint	(D,x,y,mInv);
			XRC.ray_query		(pCreator->ObjectSpace.GetStaticModel(), Device.vCameraPosition, D, fFAR);
			if (XRC.r_count())	{
				float	R		= XRC.r_begin()[0].range;
				clamp	(R,0.f,fFAR);
				R/=fFAR;
				DWORD	A		= iFloor(R*255.f);
				*p		= D3DCOLOR_XRGB(A,A,A);
			} else {
				*p		= D3DCOLOR_XRGB(0,0,255);
			}
		}
	}

	// 
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
