// Texture.cpp: implementation of the CTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

#include "image.h"
#include "texture.h"
#include "std_classes.h"
#include "xr_avi.h"


//////////////////////////////////////////////////////////////////////
// Utility pack
//////////////////////////////////////////////////////////////////////
ENGINE_API BOOL TUisAlphaPresents(D3DFORMAT f) {
	switch (f) {
	case D3DFMT_A8R8G8B8:
	case D3DFMT_A4R4G4B4:
	case D3DFMT_A8R3G3B2:
	case D3DFMT_A8P8:
	case D3DFMT_A8L8:
	case D3DFMT_A4L4:
	case D3DFMT_A8:
	case D3DFMT_DXT3:
	case D3DFMT_DXT5:
		return true;
	};
	return false;
}

ENGINE_API u32*	TUBuild32MipLevel(ETextureMipgen ALG, u32 &_w, u32 &_h, u32 &_p, u32 *pdwPixelSrc)
{
	R_ASSERT(pdwPixelSrc);
	R_ASSERT((_w%2)==0	);
	R_ASSERT((_h%2)==0	);
	R_ASSERT((_p%4)==0	);
	R_ASSERT(ALG!=tmDisable);

	u32	dwDestPitch	= (_w/2)*4;
	u32*	pNewData	= (u32*)xr_malloc( (_h/2)*dwDestPitch );
	BYTE*	pDest		= (BYTE *)pNewData;
	BYTE*	pSrc		= (BYTE *)pdwPixelSrc;

	for (u32 y = 0; y < _h; y += 2)
	{
		BYTE* pScanline = pSrc + y*_p;
		for (u32 x = 0; x < _w; x += 2)
		{
			BYTE*	p1	= pScanline + x*4;
			BYTE*	p2	= p1+4;
			BYTE*	p3	= p1+_p;
			BYTE*	p4	= p2+_p;
			u32	c1	= u32(p1[0])+u32(p2[0])+u32(p3[0])+u32(p4[0]);
			u32	c2	= u32(p1[1])+u32(p2[1])+u32(p3[1])+u32(p4[1]);
			u32	c3	= u32(p1[2])+u32(p2[2])+u32(p3[2])+u32(p4[2]);
			u32	c4	= u32(p1[3])+u32(p2[3])+u32(p3[3])+u32(p4[3]);

			u32	A	= (c4+c4/8)/4; clamp(A,(u32)0,(u32)255);
			*pDest++	= BYTE(c1/4);
			*pDest++	= BYTE(c2/4);
			*pDest++	= BYTE(c3/4);
			*pDest++	= BYTE(A);
		}
	}
	// correct for _new_ mip
	_w/=2; _h/=2; _p=_w*4;
	return pNewData;
}

ENGINE_API void	TUSelectMipLevel(u32 *w, u32 *h, u32 Q)
{
	// select LOD
	u32 d = Q+1;
	*w /= d;
	*h /= d;
	if ((*w==0)&&(*h==0)) { *w=1;  *h=1;  }
	if ((*w==0)&&(*h!=0)) { *w=1;  *h*=2; }
	if ((*w!=0)&&(*h==0)) { *w*=2; *h=1;  }
}

// minimize for memory usage
// useful for lightmaps, reflections and so forth
ENGINE_API void	TUSelectFMT_LM(D3DFORMAT *fmt)
{
	if (TUisAlphaPresents(*fmt)) {
		// select from formats containing alpha
		*fmt = D3DFMT_DXT3;
		R_CHK(D3DXCheckTextureRequirements(HW.pDevice,0,0,0,0,fmt,D3DPOOL_MANAGED));
	} else {
		*fmt = D3DFMT_DXT1;
		R_CHK(D3DXCheckTextureRequirements(HW.pDevice,0,0,0,0,fmt,D3DPOOL_MANAGED));
		if (*fmt != D3DFMT_DXT1) {
			// DXT1 - unsupported
			// select 16 bit format
			*fmt = D3DFMT_R5G6B5;
			R_CHK(D3DXCheckTextureRequirements(HW.pDevice,0,0,0,0,fmt,D3DPOOL_MANAGED));
		}
	}
}

IC u32 GetPowerOf2Plus1	(u32 v)
{
        u32 cnt=0;
        while (v) {v>>=1; cnt++; };
        return cnt;
}
IC void	Reduce				(int& w, int& h, int& l, int& skip)
{
	while ((l>1) && skip)
	{
		w /= 2;
		h /= 2;
		l -= 1;

		skip--;
	}
	if (w<1)	w=1;
	if (h<1)	h=1;
}

ENGINE_API IDirect3DBaseTexture9*	TWLoader2D
(
	u32&				mem,
	const char *		fRName,
	ETexturePF			Algorithm,
	ETextureMipgen		Mipgen,
	u32				Quality,
	float				fContrast,
	BOOL				bGrayscale,
	BOOL				bSharpen,

	// return values
	D3DFORMAT&			fmt,
	u32&				dwWidth,
	u32&				dwHeight
 )
{
	CImage					Image;
	IDirect3DTexture9*		pTexture2D		= NULL;
	IDirect3DCubeTexture9*	pTextureCUBE	= NULL;
	string256				fn;

	// validation
	R_ASSERT				(fRName);
	R_ASSERT				(fRName[0]);
	R_ASSERT				(Quality>=0 && Quality<=4);
	R_ASSERT				(fContrast>=0 && fContrast<=1);

	// make file name
	char fname[_MAX_PATH];
	strcpy(fname,fRName); if (strext(fname)) *strext(fname)=0;
	if (strstr(fname,"_bump"))								
		if (FS.exist(fn,"$game_textures$",	fname,	".dds"))	goto _BUMP;
	if (FS.exist(fn,"$level$",			fname,	".dds"))	goto _DDS;
	if (FS.exist(fn,"$game_textures$",	fname,	".dds"))	goto _DDS;

#ifdef _EDITOR
	ELog.Msg(mtError,"Can't find texture '%s'",fname);
#else
	Debug.fatal("Can't find texture '%s'",fname);
#endif
	return 0;

_DDS:
	{
		// Load and get header
		D3DXIMAGE_INFO			IMG;
		IReader* S				= FS.r_open	(fn);
		mem						= S->length	();
		R_ASSERT				(S);
		R_CHK					(D3DXGetImageInfoFromFileInMemory	(S->pointer(),S->length(),&IMG));
		if (IMG.ResourceType	== D3DRTYPE_CUBETEXTURE)			goto _DDS_CUBE;
		else														goto _DDS_2D;

_DDS_CUBE:
		{
			R_CHK(D3DXCreateCubeTextureFromFileInMemoryEx(
				HW.pDevice,
				S->pointer(),S->length(),
				D3DX_DEFAULT,
				IMG.MipLevels,0,
				IMG.Format,
				D3DPOOL_MANAGED,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				0,&IMG,0,
				&pTextureCUBE
				));
			FS.r_close				(S);

			// Log
			// OK
			dwWidth					= IMG.Width;
			dwHeight				= IMG.Height;
			fmt						= IMG.Format;
			return					pTextureCUBE;
		}
_DDS_2D:
		{
			// Check for LMAP and compress if needed
			strlwr					(fn);
			if (psDeviceFlags.is(rsCompressLMAPs)	&& strstr(fn,"lmap#"))
			{
				IMG.Format			= D3DFMT_DXT1;
			}

			// Load   SYS-MEM-surface, bound to device restrictions
			IDirect3DTexture9*		T_sysmem;
			R_CHK(D3DXCreateTextureFromFileInMemoryEx(
				HW.pDevice,
				S->pointer(),S->length(),
				D3DX_DEFAULT,D3DX_DEFAULT,
				IMG.MipLevels,0,
				IMG.Format,
				D3DPOOL_SYSTEMMEM,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				0,&IMG,0,
				&T_sysmem
				));
			FS.r_close				(S);

			// Calculate levels & dimensions
			D3DSURFACE_DESC			T_sysmem_desc0;
			R_CHK					(T_sysmem->GetLevelDesc	(0,&T_sysmem_desc0));
			int levels_2_skip		= psTextureLOD;
			int levels_exist		= T_sysmem->GetLevelCount();
			int top_width			= T_sysmem_desc0.Width;
			int top_height			= T_sysmem_desc0.Height;
			D3DFORMAT F				= T_sysmem_desc0.Format;
			Reduce					(top_width,top_height,levels_exist,levels_2_skip);

			// Create HW-surface
			R_CHK					(D3DXCreateTexture(
				HW.pDevice,
				top_width,top_height,
				levels_exist,0,F,
				D3DPOOL_MANAGED,&pTexture2D
				));

			// Copy surfaces & destroy temporary
			IDirect3DTexture9* T_src= T_sysmem;
			IDirect3DTexture9* T_dst= pTexture2D;

			int		L_src			= T_src->GetLevelCount	()-1;
			int		L_dst			= T_dst->GetLevelCount	()-1;
			for (; L_dst>=0; L_src--,L_dst--)
			{
				// Get surfaces
				IDirect3DSurface9		*S_src, *S_dst;
				R_CHK	(T_src->GetSurfaceLevel	(L_src,&S_src));
				R_CHK	(T_dst->GetSurfaceLevel	(L_dst,&S_dst));

				// Copy
				R_CHK	(D3DXLoadSurfaceFromSurface(S_dst,NULL,NULL,S_src,NULL,NULL,D3DX_FILTER_NONE,0));

				// Release surfaces
				_RELEASE				(S_src);
				_RELEASE				(S_dst);
			}

			_RELEASE				(T_sysmem);

			// OK
			dwWidth					= top_width;
			dwHeight				= top_height;
			fmt						= F;
			return					pTexture2D;
		}
	}
_BUMP:
	{
		// Load   SYS-MEM-surface, bound to device restrictions
		D3DXIMAGE_INFO			IMG;
		IReader* S				= FS.r_open	(fn);
		IDirect3DTexture9*		T_sysmem;
		R_CHK(D3DXCreateTextureFromFileInMemoryEx(
			HW.pDevice,
			S->pointer(),S->length(),
			D3DX_DEFAULT,D3DX_DEFAULT,
			D3DX_DEFAULT,0,
			D3DFMT_A8R8G8B8,
			D3DPOOL_SCRATCH,
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			0,&IMG,0,
			&T_sysmem
			));
		FS.r_close				(S);

		// Create HW-surface
		R_CHK(D3DXCreateTexture		(HW.pDevice,IMG.Width,IMG.Height,D3DX_DEFAULT,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED, &pTexture2D));
		R_CHK(D3DXComputeNormalMap	(pTexture2D,T_sysmem,0,0,D3DX_CHANNEL_RED,5.f));

		// Transfer gloss-map
		if (1)
		{
			LPDIRECT3DTEXTURE9			tDest	= pTexture2D;
			LPDIRECT3DTEXTURE9			tSrc	= T_sysmem;
			DWORD mips							= tDest->GetLevelCount();
			R_ASSERT							(mips == tSrc->GetLevelCount());

			for (DWORD i = 0; i < mips; i++)
			{
				D3DLOCKED_RECT				Rsrc,Rdst;
				D3DSURFACE_DESC				desc;

				tDest->GetLevelDesc			(i, &desc);

				tSrc->LockRect				(i,&Rsrc,0,0);
				tDest->LockRect				(i,&Rdst,0,0);

				for (u32 y = 0; y < desc.Height; y++)
				{
					for (u32 x = 0; x < desc.Width; x++)
					{
						DWORD&	pSrc	= *(((DWORD*)((BYTE*)Rsrc.pBits + (y * Rsrc.Pitch)))+x);
						DWORD&	pDst	= *(((DWORD*)((BYTE*)Rdst.pBits + (y * Rdst.Pitch)))+x);

						DWORD	mask	= DWORD(0xff) << DWORD(24);

						pDst			= (pDst& (~mask)) | (pSrc&mask);
					}
				}

				tDest->UnlockRect			(i);
				tSrc->UnlockRect			(i);
			}
		}
		_RELEASE	(T_sysmem);
		return		pTexture2D;
	}
}
