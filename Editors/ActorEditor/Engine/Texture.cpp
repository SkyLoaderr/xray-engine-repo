// Texture.cpp: implementation of the CTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

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

ENGINE_API IDirect3DTexture9*	TW_LoadTextureFromTexture
(
	IDirect3DTexture9*		t_from,
	D3DFORMAT&				t_dest_fmt,
	int						levels_2_skip,
	u32&					w,
	u32&					h
)
{
	// Calculate levels & dimensions
	IDirect3DTexture9*		t_dest			= NULL;
	D3DSURFACE_DESC			t_from_desc0	;
	R_CHK					(t_from->GetLevelDesc	(0,&t_from_desc0));
	int levels_exist		= t_from->GetLevelCount();
	int top_width			= t_from_desc0.Width;
	int top_height			= t_from_desc0.Height;
	Reduce					(top_width,top_height,levels_exist,levels_2_skip);

	// Create HW-surface
	if (D3DX_DEFAULT==t_dest_fmt)	t_dest_fmt = t_from_desc0.Format;
	R_CHK					(D3DXCreateTexture(
		HW.pDevice,
		top_width,top_height,
		levels_exist,0,t_dest_fmt,
		D3DPOOL_MANAGED,&t_dest
		));

	// Copy surfaces & destroy temporary
	IDirect3DTexture9* T_src= t_from;
	IDirect3DTexture9* T_dst= t_dest;

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

	// OK
	w						= top_width;
	h						= top_height;
	return					t_dest;
}

template	<class _It>
IC	void	TW_Iterate_1OP
(
	IDirect3DTexture9*		t_dst,
	IDirect3DTexture9*		t_src,
	_It						pred
)
{
	DWORD mips							= t_dst->GetLevelCount();
	R_ASSERT							(mips == t_src->GetLevelCount());
	for (DWORD i = 0; i < mips; i++)	{
		D3DLOCKED_RECT				Rsrc,Rdst;
		D3DSURFACE_DESC				desc,descS;

		t_dst->GetLevelDesc			(i, &desc);
		t_src->GetLevelDesc			(i, &descS);
		VERIFY						(desc.Format==descS.Format);
		VERIFY						(desc.Format==D3DFMT_A8R8G8B8);
		t_src->LockRect				(i,&Rsrc,0,0);
		t_dst->LockRect				(i,&Rdst,0,0);
		for (u32 y = 0; y < desc.Height; y++)	{
			for (u32 x = 0; x < desc.Width; x++)	{
				DWORD&	pSrc	= *(((DWORD*)((BYTE*)Rsrc.pBits + (y * Rsrc.Pitch)))+x);
				DWORD&	pDst	= *(((DWORD*)((BYTE*)Rdst.pBits + (y * Rdst.Pitch)))+x);
				pDst			= pred(pDst,pSrc);
			}
		}
		t_dst->UnlockRect			(i);
		t_src->UnlockRect			(i);
	}
}
template	<class _It>
IC	void	TW_Iterate_2OP
(
	IDirect3DTexture9*		t_dst,
	IDirect3DTexture9*		t_src0,
	IDirect3DTexture9*		t_src1,
	_It						pred
 )
{
	DWORD mips							= t_dst->GetLevelCount();
	R_ASSERT							(mips == t_src0->GetLevelCount());
	R_ASSERT							(mips == t_src1->GetLevelCount());
	for (DWORD i = 0; i < mips; i++)	{
		D3DLOCKED_RECT				Rsrc0,Rsrc1,Rdst;
		D3DSURFACE_DESC				desc,descS0,descS1;

		t_dst->GetLevelDesc			(i, &desc);
		t_src0->GetLevelDesc		(i, &descS0);
		t_src1->GetLevelDesc		(i, &descS1);
		VERIFY						(desc.Format==descS0.Format);
		VERIFY						(desc.Format==descS1.Format);
		VERIFY						(desc.Format==D3DFMT_A8R8G8B8);
		t_src0->LockRect			(i,&Rsrc0,	0,0);
		t_src1->LockRect			(i,&Rsrc1,	0,0);
		t_dst->LockRect				(i,&Rdst,	0,0);
		for (u32 y = 0; y < desc.Height; y++)	{
			for (u32 x = 0; x < desc.Width; x++)	{
				DWORD&	pSrc0	= *(((DWORD*)((BYTE*)Rsrc0.pBits + (y * Rsrc0.Pitch)))+x);
				DWORD&	pSrc1	= *(((DWORD*)((BYTE*)Rsrc1.pBits + (y * Rsrc1.Pitch)))+x);
				DWORD&	pDst	= *(((DWORD*)((BYTE*)Rdst.pBits  + (y * Rdst.Pitch)))+x);
				pDst			= pred(pDst,pSrc0,pSrc1);
			}
		}
		t_dst->UnlockRect			(i);
		t_src0->UnlockRect			(i);
		t_src1->UnlockRect			(i);
	}
}

ENGINE_API IDirect3DBaseTexture9*	TWLoader2D
(
	u32&				mem,
	const char *		fRName,

	// return values
	D3DFORMAT&			fmt,
	u32&				dwWidth,
	u32&				dwHeight
 )
{
	IDirect3DTexture9*		pTexture2D		= NULL;
	IDirect3DCubeTexture9*	pTextureCUBE	= NULL;
	string256				fn;

	// validation
	R_ASSERT				(fRName);
	R_ASSERT				(fRName[0]);
	R_ASSERT				(fContrast>=0 && fContrast<=1);

	// make file name
	char fname[_MAX_PATH];
	strcpy(fname,fRName); if (strext(fname)) *strext(fname)=0;
	if (FS.exist(fn,"$game_textures$",	fname,	".dds")	&& strstr(fname,"_bump"))	goto _BUMP;
	if (!FS.exist(fn,"$game_textures$",	fname,	".dds")	&& strstr(fname,"_bump"))	goto _BUMP_from_base;
	if (FS.exist(fn,"$level$",			fname,	".dds"))							goto _DDS;
	if (FS.exist(fn,"$game_textures$",	fname,	".dds"))							goto _DDS;

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

			// Load   SYS-MEM-surface, bound to device restrictions
			IDirect3DTexture9*		T_sysmem;
			R_CHK(D3DXCreateTextureFromFileInMemoryEx(
				HW.pDevice,S->pointer(),S->length(),
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

			pTexture2D				= TW_LoadTextureFromTexture(T_sysmem,IMG.Format,psTextureLOD,dwWidth,dwHeight);
			_RELEASE				(T_sysmem);

			// OK
			fmt						= IMG.Format;
			return					pTexture2D;
		}
	}
_BUMP:
	{
		// Load   SYS-MEM-surface, bound to device restrictions
		D3DXIMAGE_INFO			IMG;
		IReader* S				= FS.r_open	(fn);
		IDirect3DTexture9*		T_height_gloss;
		R_CHK(D3DXCreateTextureFromFileInMemoryEx(
			HW.pDevice,	S->pointer(),S->length(),
			D3DX_DEFAULT,D3DX_DEFAULT,	D3DX_DEFAULT,0,D3DFMT_A8R8G8B8,
			D3DPOOL_SYSTEMMEM,			D3DX_DEFAULT,D3DX_DEFAULT,
			0,&IMG,0,&T_height_gloss	));
		FS.r_close				(S);

		// Create HW-surface, compute normal map
		IDirect3DTexture9*	T_normal_1	= 0;
		R_CHK(D3DXCreateTexture		(HW.pDevice,IMG.Width,IMG.Height,D3DX_DEFAULT,0,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&T_normal_1));
		R_CHK(D3DXComputeNormalMap	(T_normal_1,T_height_gloss,0,0,D3DX_CHANNEL_RED,5.f));

		// Transfer gloss-map
		struct it_gloss_rev			{
			IC u32 operator(u32 d, u32 s)	{	return	color_rgba	(
					color_get_A(s)/2,	// gloss
					color_get_B(d),
					color_get_G(d),
					color_get_R(d)		);
			}
		};
		TW_Iterate_1OP		(T_normal_1,T_height_gloss,it_gloss_rev);

		// Compress
		fmt								= D3DFMT_DXT5;
		IDirect3DTexture9*	T_normal_1C	= TW_LoadTextureFromTexture(T_normal_1,fmt,psTextureLOD,dwWidth,dwHeight);
		
		// Decompress (back)
		fmt								= D3DFMT_A8R8G8B8;
		IDirect3DTexture9*	T_normal_1U	= TW_LoadTextureFromTexture(T_normal_1C,fmt,0,dwWidth,dwHeight);

		// Calculate difference
		IDirect3DTexture9*	T_normal_1D = 0;
		R_CHK(D3DXCreateTexture(HW.pDevice,dwWidth,dwHeight,T_normal_1U->GetLevelCount(),0,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&T_normal_1D));
		struct it_difference			{
			IC u32 operator(u32 d, u32 orig, u32 ucomp)	{	return	color_rgba(
				128+(int(color_get_R(orig))-int(color_get_R(ucomp)))*2,		// R-error
				128+(int(color_get_G(orig))-int(color_get_G(ucomp)))*2,		// G-error
				128+(int(color_get_B(orig))-int(color_get_B(ucomp)))*2,		// B-error
				128+(int(color_get_A(orig))-int(color_get_A(ucomp)))*2	);	// A-error	
			}
		};
		TW_Iterate_2OP		(T_normal_1D,T_normal_1,T_normal_1U,it_difference);

		// Reverse channels back + transfer heightmap
		struct it_height_rev			{
			IC u32 operator(u32 d, u32 s)	{	return	color_rgba	(
				color_get_A(d),		// diff x
				color_get_R(d),		// diff y
				color_get_G(d),		// diff z
				color_get_R(s)	);	// height
			}
		};
		TW_Iterate_1OP		(T_normal_1D,T_height_gloss,it_height_rev);

		// Compress
		fmt								= D3DFMT_DXT5;
		IDirect3DTexture9*	T_normal_2C	= TW_LoadTextureFromTexture(T_normal_1D,fmt,0,dwWidth,dwHeight);

		// release and return
		// T_normal_1C
		// T_normal_2C
		_RELEASE						(T_height_gloss	);
		_RELEASE						(T_normal_1	);
		_RELEASE						(T_normal_1U);
		_RELEASE						(T_normal_1D);
		_RELEASE						(pTexture2D	);	// ?
		pTexture2D						= T_normal_1C;

		return		pTexture2D;
	}
_BUMP_from_base:
	{
		*strstr		(fname,"_bump")	= 0;
		R_ASSERT	(FS.exist(fn,"$game_textures$",	fname,	".dds"));

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
			D3DPOOL_SYSTEMMEM,
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			0,&IMG,0,
			&T_sysmem
			));
		FS.r_close				(S);

		// Create HW-surface
		R_CHK(D3DXCreateTexture		(HW.pDevice,IMG.Width,IMG.Height,D3DX_DEFAULT,0,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM, &pTexture2D));
		R_CHK(D3DXComputeNormalMap	(pTexture2D,T_sysmem,0,D3DX_NORMALMAP_COMPUTE_OCCLUSION,D3DX_CHANNEL_LUMINANCE,5.f));

		// Transfer gloss-map
		if (1)
		{
			LPDIRECT3DTEXTURE9			tDest	= pTexture2D;
			LPDIRECT3DTEXTURE9			tSrc	= T_sysmem;
			DWORD mips							= tDest->GetLevelCount();
			R_ASSERT							(mips == tSrc->GetLevelCount());

			for (DWORD i = 0; i < mips; i++)	{
				D3DLOCKED_RECT				Rsrc,Rdst;
				D3DSURFACE_DESC				desc;

				tDest->GetLevelDesc			(i, &desc);

				tSrc->LockRect				(i,&Rsrc,0,0);
				tDest->LockRect				(i,&Rdst,0,0);

				for (u32 y = 0; y < desc.Height; y++)	{
					for (u32 x = 0; x < desc.Width; x++)	{
						DWORD&	pSrc	= *(((DWORD*)((BYTE*)Rsrc.pBits + (y * Rsrc.Pitch)))+x);
						DWORD&	pDst	= *(((DWORD*)((BYTE*)Rdst.pBits + (y * Rdst.Pitch)))+x);
						u32		occ		= color_get_A(pDst)/3;
						u32		def		= 8;
						u32		gloss	= (occ*1+def*3)/4;
						pDst			= color_rgba
							(
							gloss,
							color_get_B(pDst),
							color_get_G(pDst),
							color_get_R(pDst)
							);
					}
				}

				tDest->UnlockRect			(i);
				tSrc->UnlockRect			(i);
			}
		}
		_RELEASE	(T_sysmem);

		// Compress
		fmt								= D3DFMT_DXT5;
		IDirect3DTexture9*		T_cmp	= TW_LoadTextureFromTexture(pTexture2D,fmt,psTextureLOD,dwWidth,dwHeight);
		_RELEASE						(pTexture2D);
		pTexture2D						= T_cmp;

		return		pTexture2D;
	}
}
