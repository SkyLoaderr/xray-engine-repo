// Texture.h: interface for the CTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURE_H__0E25CF47_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_TEXTURE_H__0E25CF47_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

// Modes
enum ETexturePF {	//	Alg. for determining pixel format
	tpfDefault=0,	//	default
	tpf16,			//	16-bit
	tpf32,			//	32-bit
	tpfCompressed,	//	try compression
	tpfLM			//	try minimize mem usage
};
enum ETextureMipgen {
	tmDisable=0,	//	Disable mipmapping at all
	tmPOINT,		//	Fast - but ugly :)
	tmBOX4,			//	Apply box filter
	tmBOX16			//	Cross-smoothed box
};


// Utility pack
extern ENGINE_API IDirect3DSurface8* 		TUCreateSurfaceFromMemory(u32 w, u32 h, u32 p, D3DFORMAT fmt, void *data);
extern ENGINE_API IDirect3DTexture8* 		TUCreateTexture			(u32 *f, u32 *w, u32 *h, D3DFORMAT *fmt, u32 *m);
extern ENGINE_API BOOL						TUisAlphaPresents		(D3DFORMAT f);
extern ENGINE_API char*						TUsf2string				(D3DFORMAT f);
extern ENGINE_API void						TUSelectMipLevel		(u32 *w, u32 *h, u32 Q);
extern ENGINE_API void						TUSelectFMT_LM			(D3DFORMAT *fmt);
extern ENGINE_API void						TUProcess32_contrast	(u32 _w, u32 _h, u32 _p, float _fc, void *data);
extern ENGINE_API void						TUProcess32_grayscale	(u32 _w, u32 _h, u32 _p, void *data);
extern ENGINE_API u32*					TUBuild32MipLevel		(ETextureMipgen ALG, u32 &_w, u32 &_h, u32 &_p, u32 *pdwPixelSrc);
extern ENGINE_API void						TULoadFromMemory		(IDirect3DTexture8* pDest, u32 dwMipLevel,u32* pSrc, u32 filter);
extern ENGINE_API void						TULoadFromSurface		(IDirect3DTexture8* pDest, u32 dwMipLevel,IDirect3DSurface8* pSrc, u32 filter);
extern ENGINE_API void						TULoadFromBGR24			(IDirect3DTexture8* pDest, void *pSrc );

// Wizzard pack
extern ENGINE_API IDirect3DBaseTexture8* TWLoader2D
(
		const char *		fname,
		ETexturePF			Algorithm,
		ETextureMipgen		Mipgen,
		u32				Quality,
		float				fContrast,
		BOOL				bGrayscale,
		BOOL				bSharpen,

		// return values
		D3DFORMAT&	fmt,
		u32&				dwWidth,
		u32&				dwHeight
);

#endif // !defined(AFX_TEXTURE_H__0E25CF47_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
