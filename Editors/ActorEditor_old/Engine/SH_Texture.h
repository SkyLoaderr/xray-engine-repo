#ifndef SH_TEXTURE_H
#define SH_TEXTURE_H
#pragma once

#include "xr_resource.h"

class ENGINE_API CAviPlayerCustom;

class ENGINE_API CTexture			: public xr_resource_named				{
public:
	struct 
	{
		u32					bLoaded		: 1;
		u32					bUser		: 1;
		u32					MemoryUsage	: 30;
	}									flags;
	IDirect3DBaseTexture9*				pSurface;
	CAviPlayerCustom*					pAVI;

	// Sequence data
	u32									seqMSPF;	// milliseconds per frame
	xr_vector<IDirect3DBaseTexture9*>	seqDATA;
	BOOL								seqCycles;

	// Description
	IDirect3DBaseTexture9*				desc_cache;
	D3DSURFACE_DESC						desc;
	IC BOOL								desc_valid	()	{ return pSurface==desc_cache; }
	IC void								desc_enshure()	{ if (!desc_valid()) desc_update(); }
	void								desc_update	();
public:
	void								Load		();
	void								Unload		(void);
	void								Apply		(u32 dwStage);

	void								surface_set	(IDirect3DBaseTexture9* surf);
	IDirect3DBaseTexture9*				surface_get ();

	IC BOOL								isUser		()	{ return flags.bUser;					}
	IC u32								get_Width	()	{ desc_enshure(); return desc.Width;	}
	IC u32								get_Height	()	{ desc_enshure(); return desc.Height;	}

	CTexture							();
	virtual ~CTexture					();
};

typedef	resptr_core<CTexture,resptr_base<CTexture> >	
	ref_texture;

#endif
