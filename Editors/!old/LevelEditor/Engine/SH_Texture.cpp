#include "stdafx.h"
#pragma hdrstop

#include "ResourceManager.h"
#include "render.h"
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
	desc_cache			= 0;
	seqMSPF				= 0;
	flags.MemoryUsage	= 0;
	flags.bLoaded		= false;
	flags.bUser			= false;
	flags.seqCycles		= FALSE;
	m_material			= 1.0f;
}

CTexture::~CTexture()
{
	Unload				();

	// release external reference
	Device.Resources->_DeleteTexture	(this);	
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
	if (!flags.bLoaded)			Load	();

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
		if (flags.seqCycles)		{
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
	flags.bLoaded					= true;
	desc_cache						= 0;
	if (pSurface)					return;

	flags.bUser						= false;
	flags.MemoryUsage				= 0;
	if (0==stricmp(*cName,"$null"))	return;
	if (0!=strstr(*cName,"$user$"))	{
		flags.bUser	= true;
		return;
	}

	// Material
	if (Device.Resources->m_description->line_exist("specification",*cName))	{
		LPCSTR		descr			=	Device.Resources->m_description->r_string("specification",*cName);
		string256	bmode,bparam;	float mid;
		sscanf		(descr,"bump_mode[%[^:]:%[^]]], material[%f]",bmode,bparam,&mid);
		m_material							=	mid;
		// Msg		("%20s : bm[%s:%s] mid:%f",*cName,bmode,bparam,mid);
	}
	// Check for AVI
	string256 fn;
	if (FS.exist(fn,"$game_textures$",*cName,".avi")){
		// AVI
		pAVI = xr_new<CAviPlayerCustom>();
		if (!pAVI->Load(fn)) {
			xr_delete(pAVI);
			Debug.fatal("Can't open video stream");
		} else {
			flags.MemoryUsage	= pAVI->dwWidth*pAVI->dwHeight*4;

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
    if (FS.exist(fn,"$game_textures$",*cName,".seq")){
		// Sequence
		char buffer[256];
		destructor<IReader>	fs(FS.r_open(fn));

		flags.seqCycles	= FALSE;
		fs().r_string	(buffer);
		if (0==stricmp	(buffer,"cycled"))
		{
			flags.seqCycles	= TRUE;
			fs().r_string	(buffer);
		}
		u32 fps	= atoi(buffer);
		seqMSPF		= 1000/fps;

		while (!fs().eof())
		{
			fs().r_string(buffer);
			_Trim		(buffer);
			if (buffer[0])	{
				// Load another texture
				pSurface = ::Render->texture_load	(buffer);
				if (pSurface)	{
					// pSurface->SetPriority	(PRIORITY_LOW);
					seqDATA.push_back		(pSurface);
					flags.MemoryUsage		+= 0;
				}
			}
		}
		pSurface	= 0;
	} else
	{
		// Normal texture
		pSurface = ::Render->texture_load	(*cName);

		// Calc memory usage and preload into vid-mem
		if (pSurface) {
			// pSurface->SetPriority	(PRIORITY_NORMAL);
			flags.MemoryUsage		=	0;
		}
	}
}

void CTexture::Unload	()
{
	flags.bLoaded			= FALSE;
	if (!seqDATA.empty())	{
		for (u32 I=0; I<seqDATA.size(); I++)
			_RELEASE(seqDATA[I]);
		seqDATA.clear();
		pSurface	= 0;
	}
	_RELEASE		(pSurface);
	xr_delete		(pAVI);
}

void CTexture::desc_update	()
{
	desc_cache	= pSurface;
	if (pSurface && (D3DRTYPE_TEXTURE == pSurface->GetType()))
	{
		IDirect3DTexture9*	T	= (IDirect3DTexture9*)pSurface;
		R_CHK					(T->GetLevelDesc(0,&desc));
	}
}
