// DXT.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "dxtlib.h"
#include "TextureParams.h"
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

ENGINE_API DWORD* Build32MipLevel(DWORD &_w, DWORD &_h, DWORD &_p, DWORD *pdwPixelSrc)
{
	R_ASSERT(pdwPixelSrc);
	R_ASSERT((_w%2)==0	);
	R_ASSERT((_h%2)==0	);
	R_ASSERT((_p%4)==0	);

	DWORD	dwDestPitch	= (_w/2)*4;
	DWORD*	pNewData	= (DWORD*)malloc( (_h/2)*dwDestPitch );
	BYTE*	pDest		= (BYTE *)pNewData;
	BYTE*	pSrc		= (BYTE *)pdwPixelSrc;

	for (DWORD y = 0; y < _h; y += 2){
		BYTE* pScanline = pSrc + y*_p;
		for (DWORD x = 0; x < _w; x += 2){
			BYTE*	p1	= pScanline + x*4;
			BYTE*	p2	= p1+4;
			BYTE*	p3	= p1+_p;
			BYTE*	p4	= p2+_p;
			DWORD	c1	= DWORD(p1[0])+DWORD(p2[0])+DWORD(p3[0])+DWORD(p4[0]);
			DWORD	c2	= DWORD(p1[1])+DWORD(p2[1])+DWORD(p3[1])+DWORD(p4[1]);
			DWORD	c3	= DWORD(p1[2])+DWORD(p2[2])+DWORD(p3[2])+DWORD(p4[2]);
			DWORD	c4	= DWORD(p1[3])+DWORD(p2[3])+DWORD(p3[3])+DWORD(p4[3]);

			DWORD	A	= (c4+c4/8)/4; clamp(A,0ul,255ul);
			*pDest++	= BYTE(c1/4);
			*pDest++	= BYTE(c2/4);
			*pDest++	= BYTE(c3/4);
			*pDest++	= BYTE(A);
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
				 STextureParams* fmt, DWORD depth){

    gFileOut = _open( out_name, _O_WRONLY|_O_BINARY|_O_CREAT,_S_IWRITE);
    if (gFileOut==-1){
        fprintf(stderr, "Can't open output file %s\n", out_name);
        return false;
    }

	HRESULT hr=-1;
// convert to Options
    CompressionOptions nvOpt;
    nvOpt.MipMapsInImage 	= false;
    if (fmt->flag&EF_GENMIPMAP) 			nvOpt.MipMapType=dGenerateMipMaps;
    else if(fmt->flag&EF_USE_EXIST_MIPMAP) 	nvOpt.MipMapType=dUseExistingMipMaps;
    else if(fmt->flag&EF_NO_MIPMAP) 		nvOpt.MipMapType=dNoMipMaps;
    nvOpt.BinaryAlpha	    = !!(fmt->flag&EF_BINARYALPHA);
    nvOpt.NormalMap			= !!(fmt->flag&EF_NORMALMAP);
    nvOpt.AlphaBorder		= !!(fmt->flag&EF_ALPHAZEROBORDER);
    nvOpt.Border			= false;
    nvOpt.BorderColor.u		= 0x0;
    nvOpt.Fade				= !!(fmt->flag&EF_FADE);
    nvOpt.FadeToColor.u		= fmt->fade_color;
    nvOpt.FadeToMipMaps		= fmt->fade_mips;
    nvOpt.Dither			= !!(fmt->flag&EF_DITHER);
    nvOpt.TextureType		= dTextureType2D;
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
//-------------------

	if (nvOpt.MipMapType==dUseExistingMipMaps){
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

		for (int i=1; i<numMipmaps; i++){
			DWORD* pNewMip = Build32MipLevel(dwW,dwH,dwP,pLastMip);
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

