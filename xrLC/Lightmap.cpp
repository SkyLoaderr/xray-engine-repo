// Lightmap.cpp: implementation of the CLightmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "build.h"
#include "Lightmap.h"
#include "xrDeflector.h"
#include "xrDXTC.h"
#include "xrImage_Filter.h"

xr_vector<CLightmap*>		g_lightmaps;

extern BOOL ApplyBorders(b_texture &lm, u32 ref);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightmap::CLightmap()
{
	ZeroMemory(&lm,sizeof(lm));
}

CLightmap::~CLightmap()
{
	
}

VOID CLightmap::Capture		(CDeflector *D, int b_u, int b_v, int s_u, int s_v, BOOL bRotated, int layer)
{
	// Allocate 512x512 texture if needed
	if (0==lm.pSurface)	{
		u32	size		= lmap_size*lmap_size*sizeof(u32);
		lm.pSurface		= (u32*)(xr_malloc(size));
		lm.dwWidth		= lmap_size;
		lm.dwHeight		= lmap_size;
		lm.bHasAlpha	= FALSE;
		ZeroMemory		(lm.pSurface,size);
	}
	
	// Addressing
	xr_vector<UVtri>		tris;
	D->RemapUV			(tris,b_u+BORDER,b_v+BORDER,s_u-2*BORDER,s_v-2*BORDER,lmap_size,lmap_size,bRotated);
	
	// Capture faces and setup their coords
	for (UVIt T=tris.begin(); T!=tris.end(); T++)
	{
		UVtri&	P		= *T;
		Face	*F		= P.owner;
		F->lmap_layers.push_back	(this);
		F->AddChannel				(P.uv[0], P.uv[1], P.uv[2]);
	}
	
	// Perform BLIT
	CDeflector::Layer*	L = D->GetLayer(layer);
	R_ASSERT			(L);
	if (!bRotated) 
	{
		u32 real_H	= (L->lm.dwHeight	+ 2*BORDER);
		u32 real_W	= (L->lm.dwWidth	+ 2*BORDER);
		blit	(lm.pSurface,lmap_size,lmap_size,L->lm.pSurface,real_W,real_H,b_u,b_v,254-BORDER);
	} else {
		u32 real_H	= (L->lm.dwHeight	+ 2*BORDER);
		u32 real_W	= (L->lm.dwWidth	+ 2*BORDER);
		blit_r	(lm.pSurface,lmap_size,lmap_size,L->lm.pSurface,real_W,real_H,b_u,b_v,254-BORDER);
	}
}

//////////////////////////////////////////////////////////////////////
IC u32 convert(float a)
{
	if (a<=0)		return 0;
	else if (a>=1)	return 255;
	else			return iFloor(a*255.f);
}
IC void pixel	(int x, int y,  b_texture* T, u32 C=color_rgba(0,255,0,0))
{
	if (x<0) return; else if (x>=(int)T->dwWidth)	return;
	if (y<0) return; else if (y>=(int)T->dwHeight)	return;
	T->pSurface[y*T->dwWidth+x]	= C;
}
IC void line	( int x1, int y1, int x2, int y2, b_texture* T )
{
    int dx = _abs(x2 - x1);
    int dy = _abs(y2 - y1);
    int sx = x2 >= x1 ? 1 : -1;
    int sy = y2 >= y1 ? 1 : -1;

    if ( dy <= dx ){
        int d  = ( dy << 1 ) - dx;
        int d1 = dy << 1;
        int d2 = ( dy - dx ) << 1;

		pixel(x1,y1,T);

        for  (int x = x1 + sx, y = y1, i = 1; i <= dx; i++, x += sx){
            if ( d > 0){
                d += d2; y += sy;
            }else
                d += d1;
			pixel(x,y,T);
        }
    }else{
        int d  = ( dx << 1 ) - dy;
        int d1 = dx << 1;
        int d2 = ( dx - dy ) << 1;

		pixel(x1,y1,T);
        for  (int x = x1, y = y1 + sy, i = 1; i <= dy; i++, y += sy ){
            if ( d > 0){
                d += d2; x += sx;
            }else
                d += d1;
			pixel(x,y,T);
        }
    }
}

void CLightmap::Save()
{
	static int		lmapNameID = 0; ++lmapNameID;

	Phase			("Saving...");

	// Borders correction
	Status			("Borders...");
	for (u32 _y=0; _y<lmap_size; _y++)
	{
		for (u32 _x=0; _x<lmap_size; _x++)
		{
			u32 pixel = lm.pSurface[_y*lmap_size+_x];
			if (color_get_A(pixel)>=(254-BORDER))	pixel = (pixel&color_rgba(255,255,255,0))|color_rgba(0,0,0,255);
			else									pixel = (pixel&color_rgba(255,255,255,0));
		}
	}
	for (u32 ref=254; ref>0; ref--) {
		ApplyBorders	(lm,ref);
		Progress		(1.f - float(ref)/254.f);
	}
	Progress			(1.f);

	// Saving			(DXT5.dds)
	Status			("Compression...");
	{
		char	FN[_MAX_PATH];
		sprintf	(lm.name,"lmap#%d",lmapNameID			); 
		sprintf	(FN,"%s%s.dds",	pBuild->path,lm.name	);
		BYTE*	raw_data		= LPBYTE(lm.pSurface);
		u32	w				= lm.dwWidth;
		u32	h				= lm.dwHeight;
		u32	pitch			= w*4;

		STextureParams fmt;
		fmt.fmt					=		STextureParams::tfRGBA;
		fmt.flags.set			(STextureParams::flDitherColor,		FALSE);
		fmt.flags.set			(STextureParams::flGenerateMipMaps,	FALSE);
		DXTCompress				(FN,raw_data,w,h,pitch,&fmt,4);
	}

	xr_free			(lm.pSurface);
}
