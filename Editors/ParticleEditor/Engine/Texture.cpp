// Texture.cpp: implementation of the CTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "image.h"      
#include "texture.h"
#include "std_classes.h"
#include "xr_avi.h"
#include "xr_ini.h"
#include "xr_tokens.h"
#include "xr_trims.h"


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
/*
ENGINE_API char* TUsf2string(D3DFORMAT f) {
	switch (f) {
        case D3DFMT_R8G8B8:    return "R8G8B8";
        case D3DFMT_A8R8G8B8:  return "A8R8G8B8";
        case D3DFMT_X8R8G8B8:  return "X8R8G8B8";
        case D3DFMT_R5G6B5:    return "R5G6B5";
        case D3DFMT_R5G5B5:    return "R5G5B5";
        case D3DFMT_PALETTE4:  return "PALETTE4";
        case D3DFMT_PALETTE8:  return "PALETTE8";
        case D3DFMT_A1R5G5B5:  return "A1R5G5B5";
        case D3DFMT_X4R4G4B4:  return "X4R4G4B4";
        case D3DFMT_A4R4G4B4:  return "A4R4G4B4";
        case D3DFMT_L8:        return "L8";
        case D3DFMT_A8L8:      return "A8L8";
        case D3DFMT_U8V8:      return "U8V8";
        case D3DFMT_U5V5L6:    return "U5V5L6";
        case D3DFMT_U8V8L8:    return "U8V8L8";
        case D3DFMT_UYVY:      return "UYVY";
        case D3DFMT_YUY2:      return "YUY2";
        case D3DFMT_DXT1:      return "DXT1";
        case D3DFMT_DXT3:      return "DXT3";
        case D3DFMT_DXT5:      return "DXT5";
        case D3DFMT_R3G3B2:    return "R3G3B2";
        case D3DFMT_A8:        return "A8";
        case D3DFMT_TEXTUREMAX:return "TEXTUREMAX";
        case D3DFMT_Z16S0:     return "Z16S0";
        case D3DFMT_Z32S0:     return "Z32S0";
        case D3DFMT_Z15S1:     return "Z15S1";
        case D3DFMT_Z24S8:     return "Z24S8";
        case D3DFMT_S1Z15:     return "S1Z15";
        case D3DFMT_S8Z24:     return "S8Z24";
        case D3DFMT_DEPTHMAX:	return "DEPTHMAX";
        case D3DFMT_UNKNOWN:
        default:				return "UNKNOWN";
	}
}
*/
/*
ENGINE_API IDirect3DTexture9*	TUCreateTexture(
		u32 *f, u32 *w, u32 *h, D3DFORMAT *fmt,
		u32 *m)
{
	LPDIRECTDRAWSURFACE7	pSurf=NULL;
	R_CHK(D3DXCreateTexture(
		   HW.pDevice,
		   f,w,h,fmt,pal,
		   &pSurf, m
		   ));
	R_ASSERT(pSurf);
	return pSurf;
}

ENGINE_API LPDIRECTDRAWSURFACE7	TUCreateSurfaceFromMemory(
		u32 _w, u32 _h, u32 _p, D3DFORMAT fmt, void *data)
{
    DDSURFACEDESC2       ddsd2;
    LPDIRECTDRAWSURFACE7 lpDDS;


    // Initialize the surface description.
    ZeroMemory(&ddsd2, sizeof(DDSURFACEDESC2));
    ddsd2.dwSize = sizeof(ddsd2);
    ddsd2.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_LPSURFACE |
                    DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_CAPS;
    ddsd2.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    ddsd2.dwWidth = _w;
    ddsd2.dwHeight= _h;
    ddsd2.lPitch  = (LONG)_p;
    ddsd2.lpSurface = data;

    // Set up the pixel format for 32-bit RGB (8-8-8-8).
    TUMakeDDPixelFormat(fmt,&ddsd2.ddpfPixelFormat);

    // Create the surface
    R_CHK(HW.pDDraw->CreateSurface(&ddsd2, &lpDDS, NULL));
    R_ASSERT(lpDDS);
    return lpDDS;
}

ENGINE_API void					TULoadFromSurface	(
		LPDIRECTDRAWSURFACE7 pDest, u32 dwMipLevel,
		LPDIRECTDRAWSURFACE7 pSrc,	D3DX_FILTERTYPE filter)
{
	R_ASSERT(pDest);
	R_ASSERT(pSrc);
	R_CHK(D3DXLoadTextureFromSurface(
		HW.pDevice,
		pDest,
		dwMipLevel,
		pSrc,
		NULL,
		NULL,
		filter
		));
}

ENGINE_API LPDIRECTDRAWSURFACE7	TUGetMipLevel(LPDIRECTDRAWSURFACE7 pS, u32 L)
{
	R_ASSERT				(pS);

	LPDIRECTDRAWSURFACE7	pSub=NULL;
    DDSCAPS2				ddsCaps;
    ZeroMemory				(&ddsCaps, sizeof(ddsCaps));
    ddsCaps.dwCaps			= DDSCAPS_TEXTURE;
    ddsCaps.dwCaps2			= DDSCAPS2_MIPMAPSUBLEVEL;

	while (L) {
		R_CHK(pS->GetAttachedSurface(&ddsCaps,&pSub));	VERIFY(pSub);
		pS=pSub; pS->Release();	// decrements ref count
		L--;
	}
	return pS;
}

ENGINE_API void		TULoadFromMemory(
		LPDIRECTDRAWSURFACE7 pDest, u32 dwMipLevel,
		u32* pSrc, D3DX_FILTERTYPE filter)
{
	R_ASSERT(pDest);
	R_ASSERT(pSrc);

	LPDIRECTDRAWSURFACE7	pMip = TUGetMipLevel(pDest,dwMipLevel);

	DDSURFACEDESC2 ddsd;
	ddsd.dwSize=sizeof(ddsd);
	pMip->GetSurfaceDesc(&ddsd);

	LPDIRECTDRAWSURFACE7	pTMP =
		TUCreateSurfaceFromMemory(
			ddsd.dwWidth,ddsd.dwHeight,ddsd.dwWidth*4,D3DFMT_A8R8G8B8,pSrc);
	TULoadFromSurface(pDest,dwMipLevel,pTMP,filter);
	_RELEASE				(pTMP);
}
*/
/*
	pDest = TUGetMipLevel(pDest,dwMipLevel);

	DDSURFACEDESC2 ddsd;
	ddsd.dwSize=sizeof(ddsd);
	pDest->GetSurfaceDesc(&ddsd);


	LPDIRECTDRAWSURFACE7	pTMP =
		TUCreateSurfaceFromMemory(
			ddsd.dwWidth,ddsd.dwHeight,ddsd.dwWidth*4,D3DFMT_A8R8G8B8,pSrc);
	R_CHK(pDest->Blt(NULL,pTMP,NULL,DDBLT_WAIT,NULL));
*/

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
/*
ENGINE_API void TULoadFromBGR24(LPDIRECTDRAWSURFACE7 picSurf, void *picData )
{
    DDSURFACEDESC2	picDesc;

	VERIFY(picSurf);

    picDesc.dwSize = sizeof(picDesc);
	CHK_DX(picSurf->Lock(0,&picDesc,DDLOCK_WAIT|DDLOCK_WRITEONLY,0));

	u32	picW,picH;
	WORD	off_r,off_g,off_b;
	u32	mask_r,mask_g,mask_b;
	u32	loopW,loopH;

	picW = picDesc.dwWidth;
	picH = picDesc.dwHeight;

	VERIFY( (picDesc.ddpfPixelFormat.dwRGBBitCount % 8)==0 );

	VERIFY( picDesc.lpSurface );
	VERIFY( picDesc.ddpfPixelFormat.dwRBitMask );
	VERIFY( picDesc.ddpfPixelFormat.dwGBitMask );
	VERIFY( picDesc.ddpfPixelFormat.dwBBitMask );


	mask_r = picDesc.ddpfPixelFormat.dwRBitMask;
	mask_g = picDesc.ddpfPixelFormat.dwGBitMask;
	mask_b = picDesc.ddpfPixelFormat.dwBBitMask;

	u32 bitmapscansize = picW*3;
	if (bitmapscansize & 3) {
		bitmapscansize = (bitmapscansize & ~3) + 4;
	}

	u32 bytepixsize = picDesc.ddpfPixelFormat.dwRGBBitCount / 8;
	LPBYTE px = (LPBYTE) picDesc.lpSurface;
	LPBYTE orgdata = (LPBYTE) picData + bitmapscansize * picH;

	loopW = picW;
	loopH = picH;

	u32 addptr2 = picDesc.lPitch - loopW*bytepixsize;
	u32 addptr1 = bitmapscansize - picW*3;

	__asm{

			mov eax,mask_r
			bsr ebx,eax
			mov dx,31
			sub dx,bx
			mov off_r,dx

			mov eax,mask_g
			bsr ebx,eax
			mov dx,31
			sub dx,bx
			mov off_g,dx

			mov eax,mask_b
			bsr ebx,eax
			mov dx,31
			sub dx,bx
			mov off_b,dx
	}

	__asm{

			// WH loops prepare

			mov esi,orgdata
			mov edi,px
			mov ecx,loopH

scanloop_b24_H:

			push ecx
			mov ecx,loopW
			sub esi,bitmapscansize

scanloop_b24_W:

			mov edx,ecx

			mov ebx,0

			// B
			mov al, byte ptr [esi]
			shl eax, 24
			mov cx,  off_b
			shr eax, cl
			and eax, mask_b
			or  ebx, eax

			// G
			mov al, byte ptr [esi+1]
			shl eax,24
			mov cx,off_g
			shr eax,cl
			and eax,mask_g
			or  ebx,eax

			// R
			mov al, byte ptr [esi+2]
			shl eax,24
			mov cx,off_r
			shr eax,cl
			and eax,mask_r
			or  ebx,eax

			// store pixel

			mov ecx,bytepixsize
mvloop_b24:
			mov byte ptr [edi], bl
			shr ebx,8
			inc edi
			loop mvloop_b24


			add esi,3
			mov ecx,edx
			loop scanloop_b24_W

			// adjust scanline
			add edi,addptr2
			add esi,addptr1
			sub esi,bitmapscansize

			pop ecx
			dec ecx
			cmp ecx,0
			jz  end
			jmp scanloop_b24_H
end:
	}

	CHK_DX(picSurf->Unlock(0));
}
*/

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
#ifdef M_BORLAND
	if (Engine.FS.Exist(fn,Engine.FS.m_GameTextures.m_Path,fname,	".dds"))	goto _DDS;
	ELog.Msg(mtError,"Can't find texture '%s'",fname);
#else
	if (Engine.FS.Exist(fn,Path.Current,fname,	".dds"))	goto _DDS;
	if (Engine.FS.Exist(fn,Path.Textures,fname,	".dds"))	goto _DDS;
	Debug.fatal("Can't find texture '%s'",fname);
#endif
	return 0;

_DDS:
	{
		// Load and get header
		D3DXIMAGE_INFO			IMG;
		CStream* S				= Engine.FS.Open	(fn);
		mem						= S->Length			();
		R_ASSERT				(S);
		R_CHK					(D3DXGetImageInfoFromFileInMemory	(S->Pointer(),S->Length(),&IMG));
		if (IMG.ResourceType	== D3DRTYPE_CUBETEXTURE)			goto _DDS_CUBE;
		else														goto _DDS_2D;

_DDS_CUBE:
		{
			R_CHK(D3DXCreateCubeTextureFromFileInMemoryEx(
				HW.pDevice,
				S->Pointer(),S->Length(),
				D3DX_DEFAULT,
				IMG.MipLevels,0,
				IMG.Format,
				D3DPOOL_MANAGED,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				0,&IMG,0,
				&pTextureCUBE
				));
			Engine.FS.Close			(S);

			// Log
#ifdef DEBUG
			Msg						("* T_C [%d-%d] (%dK): %s",IMG.Width,IMG.Height,mem/1024,fn);
#endif
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
				S->Pointer(),S->Length(),
				D3DX_DEFAULT,D3DX_DEFAULT,
				IMG.MipLevels,0,
				IMG.Format,
				D3DPOOL_SYSTEMMEM,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				0,&IMG,0,
				&T_sysmem
				));
			Engine.FS.Close			(S);

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

			// Log
#ifdef DEBUG
			Msg						("* T_2 [%d-%d] (%dK): %s",top_width,top_height,mem/1024,fn);
#endif
			// OK
			dwWidth					= top_width;
			dwHeight				= top_height;
			fmt						= F;
			return					pTexture2D;
		}
	}
	return pTexture2D;
}
