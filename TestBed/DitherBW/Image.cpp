// Image.cpp: implementation of the CImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Image.h"

void CImage::Vflip()
{
	R_ASSERT(pData);
	for (DWORD y=0; y<dwHeight/2; y++)
	{
		DWORD y2 = dwHeight-y-1;
		for (DWORD x=0; x<dwWidth; x++) {
			DWORD t = GetPixel(x,y);
			PutPixel(x,y, GetPixel(x,y2));
			PutPixel(x,y2,t);
		}
	}
}
void CImage::Hflip()
{
	R_ASSERT(pData);
	for (DWORD y=0; y<dwHeight; y++)
	{
		for (DWORD x=0; x<dwWidth/2; x++) {
			DWORD x2 = dwWidth-x-1;
			DWORD t = GetPixel(x,y);
			PutPixel(x,y, GetPixel(x2,y));
			PutPixel(x2,y,t);
		}
	}
}

IC BYTE ClampColor(float a) 
{
	int c = iFloor(a);
	if (c<0) c=0; else if (c>255) c=255;
    return BYTE(c);
}

void CImage::Contrast(float _fc)
{
	R_ASSERT(pData);

    // Apply contrast correction
    char *ptr       = (char *)pData;
    float fc        = _fc*2.f;
    for (DWORD i=0; i<dwHeight; i++) {
		for (DWORD j=0; j<dwWidth; j++) {
			BYTE *p = (BYTE *) &(ptr[i*dwWidth*4 + j*4]);
			p[0] = ClampColor(128.f + fc*(float(p[0]) - 128.f)); // red
			p[1] = ClampColor(128.f + fc*(float(p[1]) - 128.f)); // green
			p[2] = ClampColor(128.f + fc*(float(p[2]) - 128.f)); // blue
		}
    }
}
void CImage::Grayscale()
{
	R_ASSERT(pData);

	char *ptr = (char *)pData;
	for (DWORD i=0; i<dwHeight; i++) {
		for (DWORD j=0; j<dwWidth; j++) {
            BYTE *p = (BYTE *) &(ptr[i*dwWidth*4 + j*4]);
			// Approximate values for each component's contribution to luminance.
			// Based upon the NTSC standard described in ITU-R Recommendation BT.709.
			float grey = float(p[0]) * 0.2125f + float(p[1]) * 0.7154f + float(p[2]) * 0.0721f;
			p[0] = p[1] = p[2] = ClampColor(grey);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
#pragma pack(push,8)
struct _fileT {
	D3DFORMAT      fmt;
	DWORD                   dwFlags;
	DWORD                   dwWidth;
	DWORD                   dwHeight;
	DWORD                   dwMipCount;

	DWORD                   dwPitch;

	DWORD                   reserved[32-6];
};
#pragma pack(pop)

void CImage::LoadT(char *name)
{
	void*	data	= DownloadFile(name);
	_fileT*	hdr     = (_fileT*)data;
	DWORD*	pixels	= (DWORD *) ((char *)data + sizeof(_fileT));
	dwWidth = hdr->dwWidth;
	dwHeight= hdr->dwHeight;
	bAlpha	= TUisAlphaPresents(hdr->fmt);

	pData	= (DWORD*)malloc(dwWidth*dwHeight*4);
	CopyMemory(pData,pixels,dwWidth*dwHeight*4);

	free(data);
}
*/

//-----------------------------------------------------------------------------
//    Load an TGA file
//-----------------------------------------------------------------------------

#define UPPER_LEFT		0x20
#define LOWER_LEFT		0x00

#define TWO_INTERLEAVE	0x40
#define FOUR_INTERLEAVE 0x80

#define BASE            0
#define RUN             1
#define LITERAL         2

#ifndef RGBA_GETALPHA
#define RGBA_GETALPHA(rgb)      DWORD((rgb) >> 24)
#define RGBA_GETRED(rgb)        DWORD(((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      DWORD(((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       DWORD((rgb) & 0xff)
#endif

#pragma pack(push,1)     // Gotta pack these structures!
struct TGAHeader
{
	BYTE	idlen;
	BYTE	cmtype;
	BYTE	imgtype;

	WORD	cmorg;
	WORD	cmlen;
	BYTE	cmes;

	short	xorg;
	short	yorg;
	short	width;
	short	height;
	BYTE	pixsize;
	BYTE	desc;
};
#pragma pack(pop)

void CImage::LoadTGA(char *name)
{
	CFileStream	TGA(name);
	TGAHeader	hdr;
	BOOL		hflip, vflip;

	TGA.Read(&hdr,sizeof(TGAHeader));

#ifdef ENGINE_BUILD
	if (hdr.imgtype!=2)			Device.Fatal("Invalid texture format (%s)",name);
#endif
	R_ASSERT(hdr.imgtype==2);							// Uncompressed
	R_ASSERT((hdr.pixsize==24) || (hdr.pixsize==32));	// 24bpp/32bpp
	R_ASSERT(btwIsPow2(hdr.width));
	R_ASSERT(btwIsPow2(hdr.height));

	// Skip funky stuff
	if (hdr.idlen)	TGA.Advance(hdr.idlen);
	if (hdr.cmlen)	TGA.Advance(hdr.cmlen*((hdr.cmes+7)/8));

	hflip		= (hdr.desc & 0x10) ? TRUE : FALSE;		// Need hflip
	vflip		= (hdr.desc & 0x20) ? FALSE: TRUE;		// Need vflip

	dwWidth		= hdr.width;
	dwHeight	= hdr.height;

	// Alloc memory
	pData		= (DWORD*)malloc(dwWidth*dwHeight*4);

	if (hdr.pixsize==24)
	{	// 24bpp
		bAlpha = FALSE;
		DWORD	pixel	= 0;
		DWORD*	ptr		= pData;
		for(int iy = 0; iy<hdr.height; ++iy) {
			for(int ix=0; ix<hdr.width; ++ix) {
				TGA.Read(&pixel,3); *ptr++=pixel;
				/*
				DWORD R = RGBA_GETRED	(pixel)/2;
				DWORD G = RGBA_GETGREEN	(pixel)/2;
				DWORD B = RGBA_GETBLUE	(pixel)/2;
				*ptr++ = D3DCOLOR_XRGB(R,G,B);
				*/
			}
		}
	}
	else
	{	// 32bpp
		bAlpha = TRUE;
		TGA.Read(pData,hdr.width*hdr.height*4);
	}
	if (vflip) Vflip();
	if (hflip) Hflip();
}
