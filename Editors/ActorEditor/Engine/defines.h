#ifndef DefinesH
#define DefinesH

ENGINE_API	extern BOOL			bDebug;

#define _RELEASE(x)			{ if(x) { (x)->Release();       (x)=NULL; } }
#define _SHOW_REF(msg, x)   { if(x) { x->AddRef(); Log(msg,u32(x->Release()));}}

// textures
enum {
	TF_ShowMipmaps		= 1<<0,
	TF_forcedword		= u32(-1)
};
ENGINE_API extern	int		psTextureLOD		;

// video
enum {
	rsFullscreen		= (1ul<<0ul),
	rsTriplebuffer		= (1ul<<1ul),
	rsClearBB			= (1ul<<2ul),
	rsNoVSync			= (1ul<<3ul),
	rsWireframe			= (1ul<<4ul),
	rsAnisotropic		= (1ul<<5ul),
	rsAntialias			= (1ul<<6ul),
	rsWarmHZB			= (1ul<<7ul),
	rsNormalize			= (1ul<<8ul),
	rsCompressLMAPs		= (1ul<<9ul),
	rsOcclusion			= (1ul<<10ul),
	rsStatistic			= (1ul<<11ul),
	rsDetails			= (1ul<<12ul),
	rsPostprocess		= (1ul<<13ul),
	rsRefresh60hz		= (1ul<<14ul),
	mtSound				= (1ul<<15ul),
	mtInput				= (1ul<<16ul),
	mtPhysics			= (1ul<<17ul)
// 20-32 bit - defined in Editor
};
ENGINE_API extern	u32			psCurrentMode		;
ENGINE_API extern	u32			psCurrentBPP		;
ENGINE_API extern	Flags32		psDeviceFlags		;

#endif
