#ifndef SH_TEXTURE_H
#define SH_TEXTURE_H
#pragma once

class ENGINE_API CAviPlayerCustom;

class ENGINE_API CTexture
{
public:
	DWORD						dwReference;
	IDirect3DTexture8*			pSurface;
	CAviPlayerCustom*			pAVI;
	DWORD						dwMemoryUsage;
	
	// Sequence data
	DWORD						seqMSPF;	// milliseconds per frame
	vector<IDirect3DTexture8*>	seqDATA;
	BOOL						seqCycles;
	
	// User-data
	BOOL						bUser;
	
	DWORD						Calculate_MemUsage(IDirect3DTexture8* T);
public:
	void						Load		(LPCSTR Name);
	void						Unload		(void);
	void						Apply		(DWORD dwStage);
	
	void						surface_set	(IDirect3DTexture8* surf);
	IDirect3DTexture8*			surface_get ();

	IC BOOL						isUser		()	{return bUser;};

	CTexture					();
	virtual ~CTexture			();
};
#endif