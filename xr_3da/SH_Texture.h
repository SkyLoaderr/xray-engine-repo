#pragma once

class ENGINE_API CAviPlayerCustom;

class ENGINE_API CTexture
{
public:
	sh_name						cName;
	DWORD						dwRefCount;
	BOOL						bMipmaps;
	IDirect3DTexture8*			pSurface;
	CAviPlayerCustom*			pAVI;
	DWORD						dwMemoryUsage;
	
	// Sequence data
	DWORD						seqMSPF;	// milliseconds per frame
	vector<IDirect3DTexture8*>	seqDATA;
	BOOL						seqCycles;

	DWORD						Calculate_MemUsage(IDirect3DTexture8* T);
public:
	void						Load		(void);
	void						Unload		(void);
	void						Apply		(DWORD dwStage);

	CTexture					(LPCSTR Name, BOOL bMipmaps);
	~CTexture					();
};

class	ENGINE_API	CTextureArray
{
public:
	typedef	svector<CTexture*,8>	VECTOR;	

	svector<VECTOR,8>	textures;

	VECTOR&				Pass		(DWORD ID)	{ return textures[ID]; }

	CTextureArray()
	{
		ZeroMemory	(this,sizeof(*this)));
	}
};
