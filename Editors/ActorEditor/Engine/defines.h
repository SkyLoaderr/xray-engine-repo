#ifndef _DEFINES_H_
#define _DEFINES_H_

ENGINE_API	extern BOOL		bDebug;

typedef	char				FILE_NAME	[ _MAX_PATH	];

struct ENGINE_API			FS_Defs
{
	const char*				GameData;
	const char*				Levels;
	const char*				Textures;
	const char*				Sounds;
	const char*				CForms;
	const char*				Meshes;
	FILE_NAME				Current;
};
extern ENGINE_API FS_Defs	Path;

// 'cause delete(NULL) is valid - there is no sence to check for NULL :)
#define _DELETE(x)			{ delete x;	(x)=NULL; }
#define _DELETEARRAY(x)		{ delete[] x;	(x)=NULL; }
#define _RELEASE(x)			{ if(x) { (x)->Release();       (x)=NULL; } }
#define _FREE(x)			{ if(x) { xr_free(x);           (x)=NULL; } }
#define THROW				throw 123456789
#define _SHOW_REF(msg, x)   { if(x) { x->AddRef(); Log(msg,x->Release());}}

// Sound
enum {
	ssWaveTrace			= (1ul<<0ul),
	ssEAX				= (1ul<<1ul),
	ss_forcedword		= u32(-1)
};
enum {
	sf_11K,
	sf_22K,
	sf_44K,
	sf_forcedword = u32(-1)
};
enum {
	sq_DEFAULT,
	sq_NOVIRT,
	sq_LIGHT,
	sq_HIGH,
	sq_forcedword = u32(-1)
};
ENGINE_API extern u32	psSoundFreq				;
ENGINE_API extern u32	psSoundModel			;
ENGINE_API extern float	psSoundVMaster			;
ENGINE_API extern float	psSoundVEffects			;
ENGINE_API extern float	psSoundVMusic			;
ENGINE_API extern float psSoundRolloff			;
ENGINE_API extern float psSoundDoppler			;
ENGINE_API extern float psSoundOcclusionScale	;
ENGINE_API extern float psSoundCull				;
ENGINE_API extern u32 psSoundFlags			;
ENGINE_API extern int	psSoundRelaxTime		;

// textures
enum {
	TF_ShowMipmaps		= 1<<0,
	TF_forcedword		= u32(-1)
};
ENGINE_API extern	int		psTextureLOD		;
ENGINE_API extern	u32	psTextureFlags		;

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
	mtSound				= (1ul<<16ul),
	mtInput				= (1ul<<17ul),
// 20-32 bit - defined in Editor
};
ENGINE_API extern	u32	psCurrentMode		;
ENGINE_API extern	u32	psCurrentBPP		;
ENGINE_API extern	u32	psDeviceFlags		;

#endif
