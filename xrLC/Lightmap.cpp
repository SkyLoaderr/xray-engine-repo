// Lightmap.cpp: implementation of the CLightmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "build.h"
#include "Lightmap.h"
#include "xrDeflector.h"
#include "xrDXTC.h"
#include "xrImage_Filter.h"

vector<CLightmap*>		g_lightmaps;

extern BOOL ApplyBorders(b_texture &lm, DWORD ref);

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
		DWORD	size	= lmap_size*lmap_size*sizeof(u32);
		lm.pSurface		= LPDWORD(malloc(size));
		lm.dwWidth		= lmap_size;
		lm.dwHeight		= lmap_size;
		lm.bHasAlpha	= FALSE;
		ZeroMemory		(lm.pSurface,size);
	}
	
	// Addressing
	vector<UVtri>		tris;
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
		DWORD real_H	= (L->lm.dwHeight	+ 2*BORDER);
		DWORD real_W	= (L->lm.dwWidth	+ 2*BORDER);
		blit	(lm.pSurface,lmap_size,lmap_size,L->lm.pSurface,real_W,real_H,b_u,b_v,254-BORDER);
	} else {
		DWORD real_H	= (L->lm.dwHeight	+ 2*BORDER);
		DWORD real_W	= (L->lm.dwWidth	+ 2*BORDER);
		blit_r	(lm.pSurface,lmap_size,lmap_size,L->lm.pSurface,real_W,real_H,b_u,b_v,254-BORDER);
	}
}

//////////////////////////////////////////////////////////////////////
IC DWORD convert(float a)
{
	if (a<=0)		return 0;
	else if (a>=1)	return 255;
	else			return iFloor(a*255.f);
}
IC void pixel	(int x, int y,  b_texture* T, DWORD C=RGBA_MAKE(0,255,0,0))
{
	if (x<0) return; else if (x>=(int)T->dwWidth)	return;
	if (y<0) return; else if (y>=(int)T->dwHeight)	return;
	T->pSurface[y*T->dwWidth+x]	= C;
}
IC void line	( int x1, int y1, int x2, int y2, b_texture* T )
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
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

	// Borders correction
	for (DWORD _y=0; _y<lmap_size; _y++)
	{
		for (DWORD _x=0; _x<lmap_size; _x++)
		{
			DWORD pixel = lm.pSurface[_y*lmap_size+_x];
			if (RGBA_GETALPHA(pixel)>=(254-BORDER))	pixel = (pixel&RGBA_MAKE(255,255,255,0))|RGBA_MAKE(0,0,0,255);
			else									pixel = (pixel&RGBA_MAKE(255,255,255,0));
		}
	}
	for (DWORD ref=254; ref>0; ref--) ApplyBorders(lm,ref);

	// Saving			(DXT5.dds)
	{
		char	FN[_MAX_PATH];
		sprintf	(lm.name,"L#%d",lmapNameID						);
		sprintf	(FN,"%s%s.dds",	pBuild->path.c_str(),lm.name	);
		BYTE*	raw_data		= LPBYTE(lm.pSurface);
		DWORD	w				= lm.dwWidth;
		DWORD	h				= lm.dwHeight;
		DWORD	pitch			= w*4;

		STextureParams fmt;
		fmt.fmt					=		STextureParams::tfDXT5;
		fmt.flag				&=		~STextureParams::flDitherColor;
		fmt.flag				&=		~STextureParams::flGenerateMipMaps;
		DXTCompress				(FN,raw_data,w,h,pitch,&fmt,4);
	}

	_FREE			(lm.pSurface);
}
