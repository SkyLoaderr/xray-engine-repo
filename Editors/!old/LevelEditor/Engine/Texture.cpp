// Texture.cpp: implementation of the CTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

// #include "std_classes.h"
// #include "xr_avi.h"


//////////////////////////////////////////////////////////////////////
// Utility pack
//////////////////////////////////////////////////////////////////////
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

IDirect3DTexture9*	TW_LoadTextureFromTexture
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
	const _It				pred
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
	const _It				pred
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

IC u32 it_gloss_rev		(u32 d, u32 s)	{	return	color_rgba	(
	color_get_A(s)/2,	// gloss
	color_get_B(d),
	color_get_G(d),
	color_get_R(d)		);
}
IC u32 it_gloss_rev_base(u32 d, u32 s)	{	
	u32		occ		= color_get_A(d)/3;
	u32		def		= 8;
	u32		gloss	= (occ*1+def*3)/4;
	return	color_rgba	(
		gloss,			// gloss
		color_get_B(d),
		color_get_G(d),
		color_get_R(d)
	);
}
IC u32 it_difference	(u32 d, u32 orig, u32 ucomp)	{	return	color_rgba(
	128+(int(color_get_R(orig))-int(color_get_R(ucomp)))*2,		// R-error
	128+(int(color_get_G(orig))-int(color_get_G(ucomp)))*2,		// G-error
	128+(int(color_get_B(orig))-int(color_get_B(ucomp)))*2,		// B-error
	128+(int(color_get_A(orig))-int(color_get_A(ucomp)))*2	);	// A-error	
}
IC u32 it_height_rev	(u32 d, u32 s)	{	return	color_rgba	(
	color_get_A(d),		// diff x
	color_get_R(d),		// diff y
	color_get_G(d),		// diff z
	color_get_R(s)	);	// height
}

IDirect3DBaseTexture9*	CRender::texture_load(LPCSTR fRName)
{
	IDirect3DTexture9*		pTexture2D		= NULL;
	IDirect3DCubeTexture9*	pTextureCUBE	= NULL;
	string256				fn;
	u32						dwWidth,dwHeight;
	D3DFORMAT				fmt;

	// validation
	R_ASSERT				(fRName);
	R_ASSERT				(fRName[0]);

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
		IReader*	S			= FS.r_open	(fn);
		u32			mem			= S->length	();
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
		TW_Iterate_1OP		(T_normal_1,T_height_gloss,it_gloss_rev);

		// Compress
		fmt								= D3DFMT_DXT5;
		IDirect3DTexture9*	T_normal_1C	= TW_LoadTextureFromTexture(T_normal_1,fmt,psTextureLOD,dwWidth,dwHeight);
		
#if RENDER==R_R2		
		// Decompress (back)
		fmt								= D3DFMT_A8R8G8B8;
		IDirect3DTexture9*	T_normal_1U	= TW_LoadTextureFromTexture(T_normal_1C,fmt,0,dwWidth,dwHeight);

		// Calculate difference
		IDirect3DTexture9*	T_normal_1D = 0;
		R_CHK(D3DXCreateTexture(HW.pDevice,dwWidth,dwHeight,T_normal_1U->GetLevelCount(),0,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&T_normal_1D));
		TW_Iterate_2OP		(T_normal_1D,T_normal_1,T_normal_1U,it_difference);

		// Reverse channels back + transfer heightmap
		TW_Iterate_1OP		(T_normal_1D,T_height_gloss,it_height_rev);

		// Compress
		fmt								= D3DFMT_DXT5;
		IDirect3DTexture9*	T_normal_2C	= TW_LoadTextureFromTexture(T_normal_1D,fmt,0,dwWidth,dwHeight);
		_RELEASE						(T_normal_1U	);
		_RELEASE						(T_normal_1D	);

		// 
		ref_texture			t_temp		= Device->Resources->_CreateTexture	();
#endif

		// release and return
		// T_normal_1C	- normal.gloss,		reversed
		// T_normal_2C	- 2*error.height,	non-reversed
		_RELEASE						(T_height_gloss	);
		_RELEASE						(T_normal_1		);
		return		T_normal_1C;
	}
_BUMP_from_base:
	{
		*strstr		(fname,"_bump")	= 0;
		R_ASSERT	(FS.exist(fn,"$game_textures$",	fname,	".dds"));

		// Load   SYS-MEM-surface, bound to device restrictions
		D3DXIMAGE_INFO			IMG;
		IReader* S				= FS.r_open	(fn);
		IDirect3DTexture9*		T_base;
		R_CHK(D3DXCreateTextureFromFileInMemoryEx(
			HW.pDevice,	S->pointer(),S->length(),
			D3DX_DEFAULT,D3DX_DEFAULT,	D3DX_DEFAULT,0,D3DFMT_A8R8G8B8,
			D3DPOOL_SYSTEMMEM,			D3DX_DEFAULT,D3DX_DEFAULT,
			0,&IMG,0,&T_base	));
		FS.r_close				(S);

		// Create HW-surface
		IDirect3DTexture9*	T_normal_1	= 0;
		R_CHK(D3DXCreateTexture		(HW.pDevice,IMG.Width,IMG.Height,D3DX_DEFAULT,0,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM, &T_normal_1));
		R_CHK(D3DXComputeNormalMap	(T_normal_1,T_base,0,D3DX_NORMALMAP_COMPUTE_OCCLUSION,D3DX_CHANNEL_LUMINANCE,5.f));

		// Transfer gloss-map
		TW_Iterate_1OP				(pTexture2D,T_base,it_gloss_rev_base);

		// Compress
		fmt								= D3DFMT_DXT5;
		IDirect3DTexture9*	T_normal_1C	= TW_LoadTextureFromTexture(T_normal_1,fmt,psTextureLOD,dwWidth,dwHeight);

		/*
		// Decompress (back)
		fmt								= D3DFMT_A8R8G8B8;
		IDirect3DTexture9*	T_normal_1U	= TW_LoadTextureFromTexture(T_normal_1C,fmt,0,dwWidth,dwHeight);

		// Calculate difference
		IDirect3DTexture9*	T_normal_1D = 0;
		R_CHK(D3DXCreateTexture(HW.pDevice,dwWidth,dwHeight,T_normal_1U->GetLevelCount(),0,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&T_normal_1D));
		TW_Iterate_2OP		(T_normal_1D,T_normal_1,T_normal_1U,it_difference);

		// Reverse channels back + transfer heightmap
		TW_Iterate_1OP		(T_normal_1D,T_height_gloss,it_height_rev);

		// Compress
		fmt								= D3DFMT_DXT5;
		IDirect3DTexture9*	T_normal_2C	= TW_LoadTextureFromTexture(T_normal_1D,fmt,0,dwWidth,dwHeight);
		*/

		_RELEASE						(T_base);
		_RELEASE						(T_normal_1);
		return		T_normal_1C;
	}
}
