// Texture.cpp: implementation of the CTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "image.h"
#include "xr_func.h"
#include "xr_creator.h"
#include "x_ray.h"
#include "texture.h"
#include "std_classes.h"
#include "xr_avi.h"
#include "xr_ini.h"
#include "xr_tokens.h"
#include "xr_trims.h"

#define		PRIORITY_HIGH	12
#define		PRIORITY_NORMAL	8
#define		PRIORITY_LOW	4

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTexture::CTexture(const char* Name, BOOL _bMipmaps)
{
	R_ASSERT			(Name);
	R_ASSERT			(strlen(Name)<64);
	strcpy				(cName,Name);

	dwRefCount			= 0;
	pSurface			= NULL;
	pAVI				= NULL;
	bMipmaps			= _bMipmaps;
}

CTexture::~CTexture() {
	R_ASSERT(dwRefCount==0);
}

void CTexture::Apply(DWORD dwStage)
{
	if (pAVI && pAVI->NeedUpdate()) 
	{
		// AVI
		D3DLOCKED_RECT R;
		R_CHK	(pSurface->LockRect(0,&R,NULL,0));
		R_ASSERT(R.Pitch == int(pAVI->dwWidth*4));
		R_ASSERT(pAVI->DecompressFrame(LPDWORD(R.pBits)));
		R_CHK	(pSurface->UnlockRect(0));
	} else if (!seqDATA.empty()) {
		// SEQ
		DWORD	frame		= Device.dwTimeGlobal/seqMSPF;
		DWORD	frame_data	= seqDATA.size();
		if (seqCycles)		{
			DWORD	frame_id	= frame%(frame_data*2);
			if (frame_id>=frame_data)	frame_id = (frame_data-1) - (frame_id%frame_data);
			pSurface 			= seqDATA[frame_id];
		} else {
			DWORD	frame_id	= frame%frame_data;
			pSurface 			= seqDATA[frame_id];
		}
	}
	CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
	Device.Statistic.dwTexture_Changes++;
}

void CTexture::Load()
{
	if (pSurface)					return;
	
	dwMemoryUsage					= 0;
	if (0==stricmp(cName,"$null"))	return;
	
	// Check for AVI
	FILE_NAME fn;
	if (Engine.FS.Exist(fn,Path.Textures,cName,".avi"))
	{
		// AVI
		pAVI = new CAviPlayerCustom;
		if (!pAVI->Load(fn)) {
			_DELETE(pAVI);
			Device.Fatal("Can't open video stream");
		} else {
			dwMemoryUsage	= pAVI->dwWidth*pAVI->dwHeight*4;

			// Now create texture
			HRESULT hrr = HW.pDevice->CreateTexture(
				pAVI->dwWidth,pAVI->dwHeight,1,0,D3DFMT_X8R8G8B8,D3DPOOL_MANAGED,
				&pSurface
				);
			if (FAILED(hrr)) 
			{
				Device.Fatal("Invalid video stream");
				R_CHK(hrr);
				_DELETE(pAVI);
				pSurface = 0;
			}

		}
	} else if (Engine.FS.Exist(fn,Path.Textures,cName,".seq"))
	{
		// Sequence
		char buffer[256];
		CFileStream fs(fn);

		seqCycles	= FALSE;
		fs.Rstring	(buffer);
		if (0==stricmp	(buffer,"cycled"))
		{
			seqCycles	= TRUE;
			fs.Rstring	(buffer);
		}
		DWORD fps	= atoi(buffer);
		seqMSPF		= 1000/fps;

		while (!fs.Eof())
		{
			fs.Rstring	(buffer);
			_Trim		(buffer);
			if (buffer[0])	
			{
				// Load another texture
				D3DFORMAT f; DWORD W,H;
				pSurface = TWLoader2D
					(
					buffer,
					tpfCompressed,
					bMipmaps?tmBOX4:tmDisable,
					psTextureLOD,
					0.5f,
					false,
					false,
					
					// return values
					f,W,H
					);
				if (pSurface)	{
					pSurface->SetPriority	(PRIORITY_LOW);
					seqDATA.push_back		(pSurface);
					dwMemoryUsage			+= Calculate_MemUsage	(pSurface);
				}
			}
		}
		pSurface	= 0;
	} else 
	{
		// Normal texture
		D3DFORMAT f; DWORD W,H;
		pSurface = TWLoader2D
			(
			cName,
			tpfCompressed,
			bMipmaps?tmBOX4:tmDisable,
			psTextureLOD,
			0.5f,
			false,
			false,
			
			// return values
			f,W,H
			);

		// Calc memory usage and preload into vid-mem
		if (pSurface) {
			pSurface->SetPriority	(PRIORITY_NORMAL);
///			pSurface->PreLoad		();
			dwMemoryUsage			=	Calculate_MemUsage(pSurface);
		}
	}
}

DWORD CTexture::Calculate_MemUsage	(IDirect3DTexture8* T)
{
	DWORD dwMemory	= 0;
	if (T) {
		for (DWORD L=0; L<T->GetLevelCount(); L++)
		{
			D3DSURFACE_DESC	desc;
			R_CHK			(T->GetLevelDesc(L,&desc));
			dwMemory		+= desc.Size;
		}
	}
	return dwMemory;
}

void CTexture::Unload()
{
	if (!seqDATA.empty())	
	{
		for (DWORD I=0; I<seqDATA.size(); I++)
			_RELEASE(seqDATA[I]);
		seqDATA.clear();
		pSurface	= 0;
	}
	_RELEASE	(pSurface);
	_DELETE		(pAVI);
}

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
ENGINE_API IDirect3DTexture8*	TUCreateTexture(
		DWORD *f, DWORD *w, DWORD *h, D3DFORMAT *fmt,
		DWORD *m)
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
		DWORD _w, DWORD _h, DWORD _p, D3DFORMAT fmt, void *data)
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
		LPDIRECTDRAWSURFACE7 pDest, DWORD dwMipLevel,
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

ENGINE_API LPDIRECTDRAWSURFACE7	TUGetMipLevel(LPDIRECTDRAWSURFACE7 pS, DWORD L)
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
		LPDIRECTDRAWSURFACE7 pDest, DWORD dwMipLevel,
		DWORD* pSrc, D3DX_FILTERTYPE filter)
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

ENGINE_API DWORD*	TUBuild32MipLevel(ETextureMipgen ALG, DWORD &_w, DWORD &_h, DWORD &_p, DWORD *pdwPixelSrc)
{
	R_ASSERT(pdwPixelSrc);
	R_ASSERT((_w%2)==0	);
	R_ASSERT((_h%2)==0	);
	R_ASSERT((_p%4)==0	);
	R_ASSERT(ALG!=tmDisable);

	DWORD	dwDestPitch	= (_w/2)*4;
	DWORD*	pNewData	= (DWORD*)malloc( (_h/2)*dwDestPitch );
	BYTE*	pDest		= (BYTE *)pNewData;
	BYTE*	pSrc		= (BYTE *)pdwPixelSrc;

	for (DWORD y = 0; y < _h; y += 2)
	{
		BYTE* pScanline = pSrc + y*_p;
		for (DWORD x = 0; x < _w; x += 2)
		{
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
	// correct for new
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

	DWORD	picW,picH;
	WORD	off_r,off_g,off_b;
	DWORD	mask_r,mask_g,mask_b;
	DWORD	loopW,loopH;

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

	DWORD bitmapscansize = picW*3;
	if (bitmapscansize & 3) {
		bitmapscansize = (bitmapscansize & ~3) + 4;
	}

	DWORD bytepixsize = picDesc.ddpfPixelFormat.dwRGBBitCount / 8;
	LPBYTE px = (LPBYTE) picDesc.lpSurface;
	LPBYTE orgdata = (LPBYTE) picData + bitmapscansize * picH;

	loopW = picW;
	loopH = picH;

	DWORD addptr2 = picDesc.lPitch - loopW*bytepixsize;
	DWORD addptr1 = bitmapscansize - picW*3;

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

ENGINE_API void	TUSelectMipLevel(DWORD *w, DWORD *h, DWORD Q)
{
	// select LOD
	DWORD d = Q+1;
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

IC DWORD GetPowerOf2Plus1(DWORD v)
{
        DWORD cnt=0;
        while (v) {v>>=1; cnt++; };
        return cnt;
}

ENGINE_API IDirect3DTexture8*	TWLoader2D(
		const char *		fRName,
		ETexturePF			Algorithm,
		ETextureMipgen		Mipgen,
		DWORD				Quality,
		float				fContrast,
		BOOL				bGrayscale,
		BOOL				bSharpen,

		// return values
		D3DFORMAT&			fmt,
		DWORD&				dwWidth,
		DWORD&				dwHeight
		)
{
	CImage					Image;
	DWORD					dwMipCount	= 9;
	IDirect3DTexture8*		pTexture	= NULL;
	FILE_NAME				fn;

	// validation
	R_ASSERT				(fRName);
	R_ASSERT				(fRName[0]);
	R_ASSERT				(Quality>=0 && Quality<=4);
	R_ASSERT				(fContrast>=0 && fContrast<=1);

	// make file name
	char fname[_MAX_PATH];
	strcpy(fname,fRName); if (strext(fname)) *strext(fname)=0;
	if (Engine.FS.Exist(fn,Path.Current,fname,	".dds"))	goto _DDS;
	if (Engine.FS.Exist(fn,Path.Textures,fname,	".dds"))	goto _DDS;
	if (Engine.FS.Exist(fn,Path.Current,fname,	".tga"))	goto _TGA;
	if (Engine.FS.Exist(fn,Path.Textures,fname,	".tga"))	goto _TGA;
	Device.Fatal("Can't find texture '%s'",fname);
	return 0;

_DDS:
	D3DXIMAGE_INFO IMG;
	
	R_CHK(D3DXGetImageInfoFromFile(fn,&IMG));
	R_CHK(D3DXCreateTextureFromFileEx(
		HW.pDevice,
		fn,
		D3DX_DEFAULT,D3DX_DEFAULT,
		IMG.MipLevels,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		&IMG,
		0,
		&pTexture
		));

	dwWidth		= IMG.Width;
	dwHeight	= IMG.Height;
	fmt			= IMG.Format;
	return		pTexture;

_TGA:
	Image.LoadTGA(fn);

	if (fabsf(fContrast-.5f)>.06f)	Image.Contrast(fContrast);
	if (bGrayscale)					Image.Grayscale();

	// find optimal format
	if (pSettings->LineExists("textures",fname))
	{
		static xr_token				token		[ ]={
			{ "16",					tpf16			},
			{ "32",					tpf32			},
			{ "DXTC",				tpfCompressed	},
			{ 0,					0				}
		};

		Algorithm = (ETexturePF)pSettings->ReadTOKEN("textures",fname,token);
	}

	switch (Algorithm) {
	case tpf16:
		fmt = Image.bAlpha?D3DFMT_A4R4G4B4:D3DFMT_R5G6B5;
		break;
	case tpf32:
	case tpfLM:
		fmt = Image.bAlpha?D3DFMT_A8R8G8B8:D3DFMT_X8R8G8B8;
		if (Algorithm==tpfLM) TUSelectFMT_LM(&fmt);
		break;
	case tpfDefault:
	case tpfCompressed:
	default:
		fmt = Image.bAlpha?D3DFMT_DXT3:D3DFMT_DXT1;
		break;
	}
	if (fmt==D3DFMT_DXT1 && HW.Caps.bForceDXT3)
		fmt = D3DFMT_DXT3;
	
	dwWidth = Image.dwWidth;
	dwHeight= Image.dwHeight;
	if (Mipgen!=tmDisable)
		TUSelectMipLevel(&dwWidth,&dwHeight,Quality);

	// correct for device & create surface
	if (Mipgen==tmDisable)
	{
		R_CHK(HW.pDevice->CreateTexture(
			dwWidth,dwHeight,1,0,fmt,D3DPOOL_MANAGED,&pTexture
			));
	} else {
		R_CHK(D3DXCreateTexture( 
			HW.pDevice,dwWidth,dwHeight,
			D3DX_DEFAULT,
			0,fmt,D3DPOOL_MANAGED,&pTexture));
	}
	VERIFY2(pTexture,fname);

	// fill texture with data from BGRA
	dwMipCount = pTexture->GetLevelCount();
	if (dwMipCount<=1)
	{
		// no mip-maps
		IDirect3DSurface8*	pTMP;
		R_CHK(pTexture->GetSurfaceLevel(0,&pTMP));

		RECT RC = {0,0,dwWidth,dwHeight};
		R_CHK(D3DXLoadSurfaceFromMemory(
			pTMP,0,0,
			Image.pData,
			Image.bAlpha?D3DFMT_A8R8G8B8:D3DFMT_X8R8G8B8,
			Image.dwWidth*4,
			0,
			&RC,
			D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER,
			0
			));
		_RELEASE				(pTMP);
	} else {
		// dwMipCount sometimes returned incorrectly
		// if (dwMipCount==0) dwMipCount=GetPowerOf2Plus1(_MIN(dwWidth,dwHeight));

		// 'cause of quality reducing we have to skip some levels :(
		DWORD dwP			= Image.dwWidth*4;
		DWORD dwW			= Image.dwWidth;
		DWORD dwH			= Image.dwHeight;
		DWORD *pImagePixels	= (DWORD *)malloc(dwH*dwP);
		DWORD *pNewPixels	= NULL;

		CopyMemory			(pImagePixels,Image.pData,dwH*dwP);
		while (dwWidth!=dwW) {
			pNewPixels=TUBuild32MipLevel(Mipgen,dwW,dwH,dwP,pImagePixels);
			free(pImagePixels); pImagePixels=pNewPixels; pNewPixels=NULL;
		}

		// NOW:
		//		pImagePixels	- contains data for the first mip-level of texture
		//		pNewPixels		- NULL
		//		dwW,dwH,dwP		- are correct
		for (DWORD i=0; i<dwMipCount; i++) {
			IDirect3DSurface8*	pTMP;
			R_CHK(pTexture->GetSurfaceLevel(i,&pTMP));
			RECT RC = {0,0,dwW,dwH};
			R_CHK(D3DXLoadSurfaceFromMemory(
				pTMP,0,0,
				pImagePixels,
				Image.bAlpha?D3DFMT_A8R8G8B8:D3DFMT_X8R8G8B8,
				dwP,
				0,
				&RC,
				D3DX_FILTER_NONE,
				0
				));
			_RELEASE				(pTMP);
			
			if  (dwW>1 && dwH>1) {
				// not the last level
				pNewPixels=TUBuild32MipLevel(Mipgen, dwW,dwH,dwP,pImagePixels);
				free(pImagePixels); pImagePixels=pNewPixels; pNewPixels=NULL;
			}
		}
		// in the most cases it is not freed now
		_FREE(pImagePixels);
	}

	return pTexture;
}
