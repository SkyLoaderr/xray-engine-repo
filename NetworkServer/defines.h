#ifndef _DEFINES_H_
#define _DEFINES_H_

ENGINE_API	extern BOOL		bDebug;

typedef	char				FILE_NAME	[ _MAX_PATH	];

struct ENGINE_API			FS_Defs
{
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
#define _FREE(x)			{ if(x) { free(x);              (x)=NULL; } }
#define THROW				throw 123456789
#define _SHOW_REF(msg, x)   { VERIFY(x); x->AddRef(); Log(msg,x->Release()); }

// Sound
enum ESndFreq{
	sf_11K,
	sf_22K,
	sf_44K,
	sf_forcedword = DWORD(-1)
};
enum ESndQuality{
	sq_DEFAULT,
	sq_NOVIRT,
	sq_LIGHT,
	sq_HIGH,
	sq_forcedword = DWORD(-1)
};
ENGINE_API extern DWORD	psSoundFreq		;
ENGINE_API extern DWORD	psSoundModel	;
ENGINE_API extern float	psSoundVMaster	;
ENGINE_API extern float	psSoundVEffects	;
ENGINE_API extern float	psSoundVMusic	;

// textures
enum {
	TF_ShowMipmaps		= 1<<0,
	TF_forcedword		= DWORD(-1)
};
ENGINE_API extern	DWORD	psTextureMipFilter	;
ENGINE_API extern	int		psTextureLOD		;
ENGINE_API extern	DWORD	psTextureFlags		;

// video
enum {
	rsFullscreen		= (1ul<<0ul),
	rsTriplebuffer		= (1ul<<1ul),
	rsClearBB			= (1ul<<2ul),
	rsNoVSync			= (1ul<<3ul),
	rsWireframe			= (1ul<<4ul),
	rsAntialias			= (1ul<<5ul),
	rsNormalize			= (1ul<<6ul),
	rsOverdrawView		= (1ul<<7ul),
	rsOcclusion			= (1ul<<8ul),
	rsDepthEnhance		= (1ul<<9ul),
	rsStatistic			= (1ul<<10ul),
	mtSound				= (1ul<<24ul),
	mtInput				= (1ul<<25ul)
};
ENGINE_API extern	DWORD	psCurrentMode		;
ENGINE_API extern	DWORD	psCurrentBPP		;
ENGINE_API extern	DWORD	psDeviceFlags		;

// Network
#define MAX_PLAYER_NAME			14
#define MAX_SESSION_NAME		256
extern struct _GUID g_AppGUID;
ENGINE_API extern	DWORD	psNetProtocol		;
ENGINE_API extern	char	psName[12]			;


#endif
