#include "stdafx.h"
#pragma hdrstop

#include "ResourceManager.h"
#include "render.h"
//#include "xr_avi.h"
#include "tntQAVI.h"

#define		PRIORITY_HIGH	12
#define		PRIORITY_NORMAL	8
#define		PRIORITY_LOW	4

void resptrcode_texture::create(LPCSTR _name)
{
	_set(Device.Resources->_CreateTexture(_name));
}


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
		R_ASSERT(R.Pitch == int(pAVI->m_dwWidth*4));
//		R_ASSERT(pAVI->DecompressFrame((u32*)(R.pBits)));
		BYTE* ptr; pAVI->GetFrame(&ptr);
		CopyMemory(R.pBits,ptr,pAVI->m_dwWidth*pAVI->m_dwHeight*4);
//		R_ASSERT(pAVI->GetFrame((BYTE*)(&R.pBits)));

		R_CHK	(T2D->UnlockRect(0));
	} else if (!seqDATA.empty()) {
		// SEQ
		u32	frame		= Device.TimerAsyncMM()/seqMSPF; //Device.dwTimeGlobal
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

void CTexture::Preload	()
{
	// Material
	if (Device.Resources->m_description->line_exist("specification",*cName))	{
//		if (strstr(*cName,"ston_stena"))	__asm int 3;
		LPCSTR		descr			=	Device.Resources->m_description->r_string("specification",*cName);
		string256	bmode;
		sscanf		(descr,"bump_mode[%[^]]], material[%f]",bmode,&m_material);
		if ((bmode[0]=='u')&&(bmode[1]=='s')&&(bmode[2]=='e')&&(bmode[3]==':'))
		{
			// bump-map specified
			m_bumpmap		=	bmode+4;
		}
//		Msg	("mid[%f] : %s",m_material,*cName);
	}
	/*
	// Material
	if (Device.Resources->m_description->line_exist("specification",*cName))	{
		if (strstr(*cName,"ston_stena"))	__asm int 3;
		LPCSTR		descr			=	Device.Resources->m_description->r_string("specification",*cName);
		string256	bmode,bparam;	float mid;
		sscanf		(descr,"bump_mode[%[^:]:%[^]]], material[%f]",bmode,bparam,&mid);
		m_material					=	mid;
		if ((bmode[0]=='u')&&(bmode[1]=='s')&&(bmode[2]=='e')&&(bmode[3]==0))
		{
			// bump-map specified
			m_bumpmap		=	bparam;
		}
		Msg	("mid[%f] : %s",m_material,*cName);
	}
	*/
}

void CTexture::Load		()
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

	Preload							();

	// Check for AVI
	string256 fn;
	if (FS.exist(fn,"$game_textures$",*cName,".avi")){
		// AVI
		pAVI = xr_new<CAviPlayerCustom>();

		if (!pAVI->Load(fn)) {
			xr_delete(pAVI);
			Debug.fatal("Can't open video stream");
		} else {
			flags.MemoryUsage	= pAVI->m_dwWidth*pAVI->m_dwHeight*4;

			// Now create texture
			IDirect3DTexture9*	pTexture = 0;
			HRESULT hrr = HW.pDevice->CreateTexture(
				pAVI->m_dwWidth,pAVI->m_dwHeight,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,
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
		string256 buffer;
		destructor<IReader>	fs(FS.r_open(fn));

		flags.seqCycles	= FALSE;
		fs().r_string	(buffer,sizeof(buffer));
		if (0==stricmp	(buffer,"cycled"))
		{
			flags.seqCycles	= TRUE;
			fs().r_string	(buffer,sizeof(buffer));
		}
		u32 fps	= atoi(buffer);
		seqMSPF		= 1000/fps;

		while (!fs().eof())
		{
			fs().r_string(buffer,sizeof(buffer));
			_Trim		(buffer);
			if (buffer[0])	{
				// Load another texture
				u32	mem  = 0;
				pSurface = ::Render->texture_load	(buffer,mem);
				if (pSurface)	{
					// pSurface->SetPriority	(PRIORITY_LOW);
					seqDATA.push_back		(pSurface);
					flags.MemoryUsage		+= mem;
				}
			}
		}
		pSurface	= 0;
	} else
	{
		// Normal texture
		u32	mem  = 0;
		pSurface = ::Render->texture_load	(*cName,mem);

		// Calc memory usage and preload into vid-mem
		if (pSurface) {
			// pSurface->SetPriority	(PRIORITY_NORMAL);
			flags.MemoryUsage		=	mem;
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
