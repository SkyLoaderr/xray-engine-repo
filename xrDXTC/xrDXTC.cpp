// xrDXTC.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "s3_intrf.h"
#include "xrDXTC.h"

// #pragma comment(lib,"s3tc.lib")

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

DXTC_API BOOL	__cdecl xrDXTC_Compress(
	const char *dest_file,
	DWORD		dest_format,
	BOOL		dest_mipmaps,
	DWORD*		src_data,
	DWORD		dwWidth,
	DWORD		dwHeight,
	DWORD		dxt1_alpharef
	)
{
	DDSURFACEDESC	descIn;
	DDSURFACEDESC	descOut;
	float			weight[3]	= {0.3086f, 0.6094f, 0.0820f};
	DWORD			dwFlags		= 0;

	// Mip-map gen not supported
	if (dest_mipmaps) return FALSE;

	// Select format
	S3TCsetAlphaReference(0);
	switch (dest_format)
	{
		case eDXT1: 
			dwFlags = S3TC_ENCODE_RGB_FULL;
			if (dxt1_alpharef<0xff) {
				dwFlags |= S3TC_ENCODE_RGB_ALPHA_COMPARE;
				S3TCsetAlphaReference(dxt1_alpharef);
			} else {
				dwFlags |= S3TC_ENCODE_ALPHA_NONE;
			}
			break;
		case eDXT3:
			dwFlags = S3TC_ENCODE_RGB_FULL | S3TC_ENCODE_ALPHA_EXPLICIT;
			break;
		case eDXT5:
			dwFlags = S3TC_ENCODE_RGB_FULL | S3TC_ENCODE_ALPHA_INTERPOLATED;
			break;
		default:
			return FALSE;
	}

	// Setup descIn
	descIn.dwSize = sizeof(descIn);
	descIn.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_LPSURFACE | 
			DDSD_PITCH | DDSD_PIXELFORMAT;
	descIn.dwWidth		= dwWidth;
	descIn.dwHeight		= dwHeight;
	descIn.lPitch		= dwWidth * 4;
	descIn.lpSurface	= src_data;
	descIn.ddpfPixelFormat.dwSize	= sizeof(DDPIXELFORMAT);
	descIn.ddpfPixelFormat.dwFlags	= DDPF_RGB | DDPF_ALPHAPIXELS;
	descIn.ddpfPixelFormat.dwRGBBitCount	= 32;
	descIn.ddpfPixelFormat.dwRBitMask		= 0x00ff0000;
	descIn.ddpfPixelFormat.dwGBitMask		= 0x0000ff00;
	descIn.ddpfPixelFormat.dwBBitMask		= 0x000000ff;
	descIn.ddpfPixelFormat.dwRGBAlphaBitMask= 0xff000000;

	// Setup descOut
	descOut.dwSize = sizeof(descOut);

	// Allocate memory for compressed texture
	DWORD dwCompressedSize		= S3TCgetEncodeSize(&descIn, dwFlags);
	void* pCompressedTexture	= malloc(dwCompressedSize);

	// Encode the texture
	S3TCencode(&descIn, NULL, &descOut, pCompressedTexture, dwFlags, weight);

	// Save it
	int hf = _open(dest_file,_O_BINARY|_O_CREAT|_O_SEQUENTIAL|_O_WRONLY,_S_IREAD|_S_IWRITE);
	if (hf<0) {
		free(pCompressedTexture);
		return FALSE;
	}
	DWORD dwMagic  = 0x20534444;
	_write(hf,&dwMagic,4);
	_write(hf,&descOut,sizeof(descOut));
	_write(hf,pCompressedTexture,dwCompressedSize);
	_close(hf);

	// Release memory and return
	free(pCompressedTexture);
	return TRUE;
}
