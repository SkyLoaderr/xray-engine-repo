#ifndef SH_TEXTURE_H
#define SH_TEXTURE_H
#pragma once

class ENGINE_API CAviPlayerCustom;

class ENGINE_API CTexture
{
public:
	u32							dwReference;
	IDirect3DBaseTexture8*			pSurface;
	CAviPlayerCustom*				pAVI;
	u32							dwMemoryUsage;
	
	// Sequence data
	u32							seqMSPF;	// milliseconds per frame
	vector<IDirect3DBaseTexture8*>	seqDATA;
	BOOL							seqCycles;
	
	// User-data
	BOOL							bUser;

	// Description
	IDirect3DBaseTexture8*			desc_cache;
	D3DSURFACE_DESC					desc;
	IC BOOL							desc_valid	()	{ return pSurface==desc_cache; } 
	IC void							desc_enshure()	{ if (!desc_valid()) desc_update(); }
	void							desc_update	();

public:
	void							Load		(LPCSTR Name);
	void							Unload		(void);
	void							Apply		(u32 dwStage);
	static u32					MemUsage	(IDirect3DBaseTexture8* T);
	
	void							surface_set	(IDirect3DBaseTexture8* surf);
	IDirect3DBaseTexture8*			surface_get ();

	IC BOOL							isUser		()	{return bUser;};

	IC u32							get_Width	()	{ desc_enshure(); return desc.Width;	}
	IC u32							get_Height	()	{ desc_enshure(); return desc.Height;	}
	IC u32							get_Depth	()	{ return 1;								}

	CTexture						();
	virtual ~CTexture				();
};
#endif