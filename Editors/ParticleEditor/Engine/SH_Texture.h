#ifndef SH_TEXTURE_H
#define SH_TEXTURE_H
#pragma once

class ENGINE_API CAviPlayerCustom;

class ENGINE_API CTexture
{
public:
	DWORD							dwReference;
	IDirect3DBaseTexture8*			pSurface;
	CAviPlayerCustom*				pAVI;
	DWORD							dwMemoryUsage;
	
	// Sequence data
	DWORD							seqMSPF;	// milliseconds per frame
	vector<IDirect3DBaseTexture8*>	seqDATA;
	BOOL							seqCycles;
	
	// User-data
	BOOL							bUser;
	
public:
	void							Load		(LPCSTR Name);
	void							Unload		(void);
	void							Apply		(DWORD dwStage);
	static DWORD					MemUsage	(IDirect3DBaseTexture8* T);
	
	void							surface_set	(IDirect3DBaseTexture8* surf);
	IDirect3DBaseTexture8*			surface_get ();

	IC BOOL							isUser		()	{return bUser;};

	CTexture						();
	virtual ~CTexture				();
};
#endif