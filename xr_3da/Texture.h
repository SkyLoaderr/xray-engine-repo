// Texture.h: interface for the CTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURE_H__0E25CF47_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_TEXTURE_H__0E25CF47_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

class ENGINE_API CAviPlayerCustom;

class ENGINE_API CTexture
{
public:
	sh_name					cName;
	DWORD					dwRefCount;
	BOOL					bMipmaps;
	IDirect3DTexture8*		pSurface;
	CAviPlayerCustom*		pAVI;
	DWORD					dwMemoryUsage;
	
	// Sequence data
	DWORD						seqMSPF;	// milliseconds per frame
	vector<IDirect3DTexture8*>	seqDATA;
	BOOL						seqCycles;

	DWORD					Calculate_MemUsage(IDirect3DTexture8* T);
public:
	void					Load		(void);
	void					Unload		(void);
	void					Apply		(DWORD dwStage);

	CTexture				(const char* Name, BOOL bMipmaps);
	~CTexture				();
};

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
extern ENGINE_API IDirect3DSurface8* 	TUCreateSurfaceFromMemory(DWORD w, DWORD h, DWORD p, D3DFORMAT fmt, void *data);
extern ENGINE_API IDirect3DTexture8* 	TUCreateTexture			(DWORD *f, DWORD *w, DWORD *h, D3DFORMAT *fmt, DWORD *m);
extern ENGINE_API BOOL					TUisAlphaPresents		(D3DFORMAT f);
extern ENGINE_API char*					TUsf2string				(D3DFORMAT f);
extern ENGINE_API void					TUSelectMipLevel		(DWORD *w, DWORD *h, DWORD Q);
extern ENGINE_API void					TUSelectFMT_LM			(D3DFORMAT *fmt);
extern ENGINE_API void					TUProcess32_contrast	(DWORD _w, DWORD _h, DWORD _p, float _fc, void *data);
extern ENGINE_API void					TUProcess32_grayscale	(DWORD _w, DWORD _h, DWORD _p, void *data);
extern ENGINE_API DWORD*				TUBuild32MipLevel		(ETextureMipgen ALG, DWORD &_w, DWORD &_h, DWORD &_p, DWORD *pdwPixelSrc);
extern ENGINE_API void					TULoadFromMemory		(IDirect3DTexture8* pDest, DWORD dwMipLevel,DWORD* pSrc, DWORD filter);
extern ENGINE_API void					TULoadFromSurface		(IDirect3DTexture8* pDest, DWORD dwMipLevel,IDirect3DSurface8* pSrc, DWORD filter);
extern ENGINE_API void					TULoadFromBGR24			(IDirect3DTexture8* pDest, void *pSrc );

// Wizzard pack

extern ENGINE_API IDirect3DTexture8* TWLoader2D(
		const char *		fname,
		ETexturePF			Algorithm,
		ETextureMipgen		Mipgen,
		DWORD				Quality,
		float				fContrast,
		BOOL				bGrayscale,
		BOOL				bSharpen,

		// return values
		D3DFORMAT&	fmt,
		DWORD&				dwWidth,
		DWORD&				dwHeight
		);

#endif // !defined(AFX_TEXTURE_H__0E25CF47_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
