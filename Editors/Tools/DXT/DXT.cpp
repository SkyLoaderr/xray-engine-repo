// DXT.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "dxtlib.h"
#include "ETextureParams.h"
#include "dds.h"
#include "d3dx.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

static HFILE gFileOut;
static HFILE gFileIn;

const DWORD fcc_DXT1 = MAKEFOURCC('D','X','T','1');
const DWORD fcc_DXT2 = MAKEFOURCC('D','X','T','2');
const DWORD fcc_DXT3 = MAKEFOURCC('D','X','T','3');
const DWORD fcc_DXT4 = MAKEFOURCC('D','X','T','4');
const DWORD fcc_DXT5 = MAKEFOURCC('D','X','T','5');

void WriteDTXnFile (DWORD count, void *buffer)
{
	if (count==sizeof(DDS_HEADER)){
	// correct DDS header
		DDS_HEADER* hdr=(DDS_HEADER*)buffer;
		if (hdr->dwSize==count){
			switch (hdr->ddspf.dwFourCC){ 
			case fcc_DXT1:
			case fcc_DXT2:
			case fcc_DXT3:
			case fcc_DXT4:
			case fcc_DXT5:
				hdr->ddspf.dwRGBBitCount = 0;
				break;
			}
		}
	}
    _write(gFileOut, buffer, count);
}


void ReadDTXnFile (DWORD count, void *buffer)
{
    _read(gFileIn, buffer, count);
}

HRESULT WriteCompressedData(void* data, int miplevel, DWORD size){
    _write(gFileOut, data, size);
	FillMemory(data,size,0xff);
	return 0;
}
  
ENGINE_API DWORD* Build32MipLevel(DWORD &_w, DWORD &_h, DWORD &_p, DWORD *pdwPixelSrc, STextureParams* fmt, float blend)
{
	R_ASSERT(pdwPixelSrc);
	R_ASSERT((_w%2)==0	);
	R_ASSERT((_h%2)==0	);
	R_ASSERT((_p%4)==0	);

	DWORD	dwDestPitch	= (_w/2)*4;
	DWORD*	pNewData	= (DWORD*)malloc( (_h/2)*dwDestPitch );
	BYTE*	pDest		= (BYTE *)pNewData;
	BYTE*	pSrc		= (BYTE *)pdwPixelSrc;

	float	mixed_a = (float) BYTE(fmt->fade_color >> 24);
	float	mixed_r = (float) BYTE(fmt->fade_color >> 16);
	float	mixed_g = (float) BYTE(fmt->fade_color >> 8);
	float	mixed_b = (float) BYTE(fmt->fade_color >> 0);

	float	inv_blend	= 1.f-blend;
	for (DWORD y = 0; y < _h; y += 2){
		BYTE* pScanline = pSrc + y*_p;
		for (DWORD x = 0; x < _w; x += 2){
			BYTE*	p1	= pScanline + x*4;
			BYTE*	p2	= p1+4;					if (1==_w)	p2 = p1;
			BYTE*	p3	= p1+_p;				if (1==_h)  p3 = p1;
			BYTE*	p4	= p2+_p;				if (1==_h)  p4 = p2;
			float	c_r	= float(DWORD(p1[0])+DWORD(p2[0])+DWORD(p3[0])+DWORD(p4[0])) / 4.f;
			float	c_g	= float(DWORD(p1[1])+DWORD(p2[1])+DWORD(p3[1])+DWORD(p4[1])) / 4.f;
			float	c_b	= float(DWORD(p1[2])+DWORD(p2[2])+DWORD(p3[2])+DWORD(p4[2])) / 4.f;
			float	c_a	= float(DWORD(p1[3])+DWORD(p2[3])+DWORD(p3[3])+DWORD(p4[3])) / 4.f;
			
			if (fmt->flag&STextureParams::flFadeToColor){
				c_r		= c_r*inv_blend + mixed_r*blend;
				c_g		= c_g*inv_blend + mixed_g*blend;
				c_b		= c_b*inv_blend + mixed_b*blend;
			}
			if (fmt->flag&STextureParams::flFadeToAlpha){
				c_a		= c_a*inv_blend + mixed_a*blend;
			}

			float	A	= (c_a+c_a/8.f); 
			int _r = int(c_r);	clamp(_r,0,255);	*pDest++	= BYTE(_r);
			int _g = int(c_g);	clamp(_g,0,255);	*pDest++	= BYTE(_g);
			int _b = int(c_b);	clamp(_b,0,255);	*pDest++	= BYTE(_b);
			int _a = int(A);	clamp(_a,0,255);	*pDest++	= BYTE(_a);
		}
	}
	_w/=2; _h/=2; _p=_w*4;
	return pNewData;
}
/*
bool IsThisNumberPowerOfTwo(unsigned int n)
{
        return (((n & (n-1)) == 0) && (n != 0));
}
*/
IC DWORD GetPowerOf2Plus1(DWORD v)
{
    DWORD cnt=0;
    while (v) {v>>=1; cnt++; };
    return cnt;
}

void FillRect(LPBYTE data, LPBYTE new_data, DWORD offs, DWORD pitch, DWORD h, DWORD full_pitch){
	for (DWORD i=0; i<h; i++) CopyMemory(data+(full_pitch*i+offs),new_data+i*pitch,pitch);
}

extern "C" __declspec(dllexport) 
bool DXTCompress(LPCSTR out_name, BYTE* raw_data, DWORD w, DWORD h, DWORD pitch, 
				 STextureParams* fmt, DWORD depth)
{
	R_ASSERT((0!=w)&&(0!=h));

    gFileOut = _open( out_name, _O_WRONLY|_O_BINARY|_O_CREAT|_O_TRUNC,_S_IWRITE);
    if (gFileOut==-1){
        fprintf(stderr, "Can't open output file %s\n", out_name);
        return false;
    }

	HRESULT hr=-1;
// convert to Options
    CompressionOptions nvOpt;
	ZeroMemory(&nvOpt,sizeof(nvOpt));
    nvOpt.bMipMapsInImage	= false;

	bool a = fmt->flag&STextureParams::flGenerateMipMaps;

    if (fmt->flag&STextureParams::flGenerateMipMaps)	nvOpt.MipMapType=dGenerateMipMaps;
    else												nvOpt.MipMapType=dNoMipMaps;
    nvOpt.bBinaryAlpha	    = !!(fmt->flag&STextureParams::flBinaryAlpha);
    nvOpt.bNormalMap		= !!(fmt->flag&STextureParams::flNormalMap);
	nvOpt.bDuDvMap			= !!(fmt->flag&STextureParams::flDuDvMap);
    nvOpt.bAlphaBorder		= !!(fmt->flag&STextureParams::flAlphaBorder);
    nvOpt.bBorder			= !!(fmt->flag&STextureParams::flColorBorder);
    nvOpt.BorderColor.u		= fmt->border_color;
    nvOpt.bFade				= !!(fmt->flag&STextureParams::flFadeToColor);
    nvOpt.bFadeAlpha		= FALSE;//fmt->flag.bFadeToAlpha;
    nvOpt.FadeToColor.u		= 0;//fmt->fade_color;
    nvOpt.FadeAmount		= 0;//fmt->fade_amount;
    nvOpt.bDitherColor		= !!(fmt->flag&STextureParams::flDitherColor);
    nvOpt.bDitherEachMIPLevel=!!(fmt->flag&STextureParams::flDitherEachMIPLevel);
    nvOpt.bGreyScale		= !!(fmt->flag&STextureParams::flGreyScale);
	nvOpt.TextureType		= (fmt->flag&STextureParams::flCubeMap)?dTextureTypeCube:dTextureType2D;
    switch(fmt->fmt){
    case STextureParams::tfDXT1: 	nvOpt.TextureFormat = dDXT1; 	break;
    case STextureParams::tfADXT1: 	nvOpt.TextureFormat = dDXT1a; 	break;
    case STextureParams::tfDXT3: 	nvOpt.TextureFormat = dDXT3; 	break;
    case STextureParams::tfDXT5: 	nvOpt.TextureFormat = dDXT5; 	break;
    case STextureParams::tf4444: 	nvOpt.TextureFormat = d4444; 	break;
    case STextureParams::tf1555: 	nvOpt.TextureFormat = d1555; 	break;
    case STextureParams::tf565: 	nvOpt.TextureFormat = d565; 	break;
    case STextureParams::tfRGB: 	nvOpt.TextureFormat = d888; 	break;
    case STextureParams::tfRGBA: 	nvOpt.TextureFormat = d8888; 	break;
    }
    switch(fmt->mip_filter){
    case STextureParams::dMIPFilterBox:		nvOpt.MIPFilterType = dMIPFilterBox;	break;
    case STextureParams::dMIPFilterCubic:	nvOpt.MIPFilterType = dMIPFilterCubic;	break;
    case STextureParams::dMIPFilterFullDFT:	nvOpt.MIPFilterType = dMIPFilterFullDFT;break;
    case STextureParams::dMIPFilterKaiser:	nvOpt.MIPFilterType = dMIPFilterKaiser; break;
    case STextureParams::dMIPFilterLinearLightKaiser:	nvOpt.MIPFilterType = dMIPFilterLinearLightKaiser; break;
    case STextureParams::dMIPFilterAdvanced: break;
	}
//-------------------
 
	if ((fmt->flag&STextureParams::flGenerateMipMaps)&&((STextureParams::dMIPFilterAdvanced==fmt->mip_filter)||(fmt->flag&STextureParams::flFadeToColor)||(fmt->flag&STextureParams::flFadeToAlpha))){
		nvOpt.MipMapType=dUseExistingMipMaps;

		LPBYTE pImagePixels=0;
		int numMipmaps=GetPowerOf2Plus1(__min(w,h));
		DWORD line_pitch=w*2*4;
		pImagePixels	= (LPBYTE)malloc(line_pitch*h);
		DWORD w_offs=0;
		DWORD dwW=w;
		DWORD dwH=h;
		DWORD dwP=pitch;
		LPDWORD pLastMip = (LPDWORD)malloc(w*h*4);
		CopyMemory(pLastMip,raw_data,w*h*4);
		FillRect(pImagePixels,(LPBYTE)pLastMip,w_offs,pitch,dwH,line_pitch);
		w_offs+=dwP;

		float	blend = 0;
		float	d = (float(fmt->fade_amount)/100.f)*numMipmaps-1;
		if (d<1.f) d=1.f;
		for (int i=1; i<numMipmaps; i++){
			if ((fmt->flag&STextureParams::flFadeToColor)||(fmt->flag&STextureParams::flFadeToAlpha)){
				blend = i/d;
				if (blend>1.f) blend=1.f;
			}
			DWORD* pNewMip = Build32MipLevel(dwW,dwH,dwP,pLastMip,fmt,blend);
			FillRect(pImagePixels,(LPBYTE)pNewMip,w_offs,dwP,dwH,line_pitch);
			_FREE(pLastMip); pLastMip=pNewMip; pNewMip=0;
			w_offs+=dwP;
		}
		_FREE(pLastMip);

		hr = nvDXTcompress(pImagePixels, w*2, h, line_pitch, &nvOpt, depth, 0);
		_FREE(pImagePixels);
	}else{
		hr = nvDXTcompress(raw_data, w, h, pitch, &nvOpt, depth, 0);
	}
    _close(gFileOut);
	if (hr!=DD_OK) return false;
	else return true;
}

