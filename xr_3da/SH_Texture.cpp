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
}

CTexture::~CTexture() {
	R_ASSERT(dwReference==0);
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

void CTexture::Load(LPCSTR cName)
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
