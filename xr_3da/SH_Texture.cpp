#include "stdafx.h"
#pragma hdrstop
           
#include "sh_texture.h"
#include "texture.h"
#include "xr_avi.h"
#include "xr_trims.h"

#define		PRIORITY_HIGH	12
#define		PRIORITY_NORMAL	8
#define		PRIORITY_LOW	4

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTexture::CTexture		()
{
	dwReference			= 1;
	pSurface			= NULL;
	pAVI				= NULL;
	dwMemoryUsage		= 0;
	desc_cache			= 0;
}

CTexture::~CTexture() 
{
	R_ASSERT(dwReference==0);
	Unload	();
}

void					CTexture::surface_set	(IDirect3DBaseTexture8* surf)
{
	if (surf)			surf->AddRef		();
	_RELEASE			(pSurface);
	pSurface			= surf;
}

IDirect3DBaseTexture8*	CTexture::surface_get	()
{
	if (pSurface)		pSurface->AddRef	();
	return pSurface;
}

void CTexture::Apply	(u32 dwStage)
{
	if (pAVI && pAVI->NeedUpdate())
	{
		R_ASSERT(D3DRTYPE_TEXTURE == pSurface->GetType());
		IDirect3DTexture8*	T2D		= (IDirect3DTexture8*)pSurface;

		// AVI
		D3DLOCKED_RECT R;
		R_CHK	(T2D->LockRect(0,&R,NULL,0));
		R_ASSERT(R.Pitch == int(pAVI->dwWidth*4));
		R_ASSERT(pAVI->DecompressFrame(LPDWORD(R.pBits)));
		R_CHK	(T2D->UnlockRect(0));
	} else if (!seqDATA.empty()) {
		// SEQ
		u32	frame		= Device.dwTimeGlobal/seqMSPF;
		u32	frame_data	= seqDATA.size();
		if (seqCycles)		{
			u32	frame_id	= frame%(frame_data*2);
			if (frame_id>=frame_data)	frame_id = (frame_data-1) - (frame_id%frame_data);
			pSurface 			= seqDATA[frame_id];
		} else {
			u32	frame_id	= frame%frame_data;
			pSurface 			= seqDATA[frame_id];
		}
	}
	CHK_DX(HW.pDevice->SetTexture(dwStage,pSurface));
}

void CTexture::Load(LPCSTR cName)
{
	desc_cache						= 0;
	if (pSurface)					return;
	Device.Shader.Evict				();

	bUser							= FALSE;
	dwMemoryUsage					= 0;
	if (0==stricmp(cName,"$null"))	return;
	if (0!=strstr(cName,"$user$"))	{
		bUser	= TRUE;
		return;
	}

	// Check for AVI
	FILE_NAME fn;
#ifndef M_BORLAND
	if (Engine.FS.Exist(fn,Path.Textures,cName,".avi"))
#else
	if (Engine.FS.Exist(fn,Engine.FS.m_GameTextures.m_Path,cName,".avi"))
#endif
	{
		// AVI
		pAVI = new CAviPlayerCustom;
		if (!pAVI->Load(fn)) {
			_DELETE(pAVI);
			Device.Fatal("Can't open video stream");
		} else {
			dwMemoryUsage	= pAVI->dwWidth*pAVI->dwHeight*4;

			// Now create texture
			IDirect3DTexture8*	pTexture = 0;
			HRESULT hrr = HW.pDevice->CreateTexture(
				pAVI->dwWidth,pAVI->dwHeight,1,0,D3DFMT_X8R8G8B8,D3DPOOL_MANAGED,
				&pTexture
				);
			pSurface	= pTexture;
			if (FAILED(hrr)) 
			{
				Device.Fatal("Invalid video stream");
				R_CHK	(hrr);
				_DELETE	(pAVI);
				pSurface = 0;
			}

		}
	} else
#ifndef M_BORLAND
    if (Engine.FS.Exist(fn,Path.Textures,cName,".seq"))
#else
    if (Engine.FS.Exist(fn,Engine.FS.m_GameTextures.m_Path,cName,".seq"))
#endif
	{
		// Sequence
		char buffer[256];
#ifdef _EDITOR
		destructor<CStream>	fs(new CFileStream(fn));
#else
		destructor<CStream>	fs(Engine.FS.Open(fn));
#endif

		seqCycles	= FALSE;
		fs().Rstring	(buffer);
		if (0==stricmp	(buffer,"cycled"))
		{
			seqCycles		= TRUE;                    
			fs().Rstring	(buffer);
		}
		u32 fps	= atoi(buffer);
		seqMSPF		= 1000/fps;

		while (!fs().Eof())
		{
			fs().Rstring(buffer);
			_Trim		(buffer);
			if (buffer[0])	
			{
				// Load another texture
				D3DFORMAT f; u32 W,H;
				pSurface = TWLoader2D
					(
					buffer,
					tpfCompressed,
					tmBOX4,
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
					dwMemoryUsage			+= MemUsage	(pSurface);
				}
			}
		}
		pSurface	= 0;
	} else 
	{
		// Normal texture
		D3DFORMAT f; u32 W,H;
		pSurface = TWLoader2D
			(
			cName,
			tpf32,
			tmBOX4,
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
			dwMemoryUsage			=	MemUsage(pSurface);
		}
	}
}

u32 CTexture::MemUsage	(IDirect3DBaseTexture8* pTexture)
{
	switch (pTexture->GetType())
	{
	case D3DRTYPE_TEXTURE:
		{
			IDirect3DTexture8*	T		= (IDirect3DTexture8*)pTexture;
			u32 dwMemory	= 0;
			if (T) {
				for (u32 L=0; L<T->GetLevelCount(); L++)
				{
					D3DSURFACE_DESC	desc;
					R_CHK			(T->GetLevelDesc(L,&desc));
					dwMemory		+= desc.Size;
				}
			}
			return	dwMemory;
		}
	default:
		return 0;
	}
}

void CTexture::Unload()
{
	if (!seqDATA.empty())
	{
		for (u32 I=0; I<seqDATA.size(); I++)
			_RELEASE(seqDATA[I]);
		seqDATA.clear();
		pSurface	= 0;
	}
	_RELEASE	(pSurface);
	_DELETE		(pAVI);
}

void CTexture::desc_update()
{
	desc_cache	= pSurface;
	if (pSurface && (D3DRTYPE_TEXTURE == pSurface->GetType()))
	{
		IDirect3DTexture8*	T	= (IDirect3DTexture8*)pSurface;
		R_CHK					(T->GetLevelDesc(0,&desc));
	}
}
