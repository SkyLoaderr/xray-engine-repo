#include "stdafx.h"
#pragma hdrstop

#include "sh_texture.h"
#include "texture.h"
#include "xr_avi.h"

#define		PRIORITY_HIGH	12
#define		PRIORITY_NORMAL	8
#define		PRIORITY_LOW	4

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTexture::CTexture		()
{
	pSurface			= NULL;
	pAVI				= NULL;
	dwMemoryUsage		= 0;
	desc_cache			= 0;
}

CTexture::~CTexture()
{
//	R_ASSERT(dwReference==0);
	Unload	();
}

void					CTexture::surface_set	(IDirect3DBaseTexture9* surf)
{
	if (surf)			surf->AddRef		();
	_RELEASE			(pSurface);
	pSurface			= surf;
}

IDirect3DBaseTexture9*	CTexture::surface_get	()
{
	if (pSurface)		pSurface->AddRef	();
	return pSurface;
}

void CTexture::Apply	(u32 dwStage)
{
	if (pAVI && pAVI->NeedUpdate())
	{
		R_ASSERT(D3DRTYPE_TEXTURE == pSurface->GetType());
		IDirect3DTexture9*	T2D		= (IDirect3DTexture9*)pSurface;

		// AVI
		D3DLOCKED_RECT R;
		R_CHK	(T2D->LockRect(0,&R,NULL,0));
		R_ASSERT(R.Pitch == int(pAVI->dwWidth*4));
		R_ASSERT(pAVI->DecompressFrame((u32*)(R.pBits)));
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

void CTexture::Load()
{
	desc_cache						= 0;
	if (pSurface)					return;

	bUser							= FALSE;
	dwMemoryUsage					= 0;
	if (0==stricmp(cName,"$null"))	return;
	if (0!=strstr(cName,"$user$"))	{
		bUser	= TRUE;
		return;
	}

	// Check for AVI
	string256 fn;
	if (FS.exist(fn,"$game_textures$",cName,".avi")){
		// AVI
		pAVI = xr_new<CAviPlayerCustom>();
		if (!pAVI->Load(fn)) {
			xr_delete(pAVI);
			Debug.fatal("Can't open video stream");
		} else {
			dwMemoryUsage	= pAVI->dwWidth*pAVI->dwHeight*4;

			// Now create texture
			IDirect3DTexture9*	pTexture = 0;
			HRESULT hrr = HW.pDevice->CreateTexture(
				pAVI->dwWidth,pAVI->dwHeight,1,0,D3DFMT_X8R8G8B8,D3DPOOL_MANAGED,
				&pTexture,NULL
				);
			pSurface	= pTexture;
			if (FAILED(hrr))
			{
				Debug.fatal	("Invalid video stream");
				R_CHK		(hrr);
				xr_delete	(pAVI);
				pSurface = 0;
			}

		}
	} else
    if (FS.exist(fn,"$game_textures$",cName,".seq")){
		// Sequence
		char buffer[256];
		destructor<IReader>	fs(FS.r_open(fn));

		seqCycles	= FALSE;
		fs().r_string	(buffer);
		if (0==stricmp	(buffer,"cycled"))
		{
			seqCycles		= TRUE;
			fs().r_string	(buffer);
		}
		u32 fps	= atoi(buffer);
		seqMSPF		= 1000/fps;

		while (!fs().eof())
		{
			fs().r_string(buffer);
			_Trim		(buffer);
			if (buffer[0])
			{
				// Load another texture
				D3DFORMAT f; u32 W,H,M;
				pSurface = TWLoader2D
					(
					M,
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
					dwMemoryUsage			+= M;
				}
			}
		}
		pSurface	= 0;
	} else
	{
		// Normal texture
		D3DFORMAT f; u32 W,H,M;
		pSurface = TWLoader2D
			(
			M,
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
			dwMemoryUsage			=	M;
		}
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
	xr_delete		(pAVI);
}

void CTexture::desc_update()
{
	desc_cache	= pSurface;
	if (pSurface && (D3DRTYPE_TEXTURE == pSurface->GetType()))
	{
		IDirect3DTexture9*	T	= (IDirect3DTexture9*)pSurface;
		R_CHK					(T->GetLevelDesc(0,&desc));
	}
}
