#ifndef DefinesH
#define DefinesH

#ifdef	DEBUG
	ENGINE_API	extern BOOL			bDebug;
#else
	#define bDebug 0
#endif

#define _RELEASE(x)			{ if(x) { (x)->Release();       (x)=NULL; } }
#define _SHOW_REF(msg, x)   { if(x) { x->AddRef(); Log(msg,u32(x->Release()));}}

// textures
ENGINE_API extern	int		psTextureLOD		;

// video
enum {
	rsFullscreen		= (1ul<<0ul),
	rsTriplebuffer		= (1ul<<1ul),
	rsClearBB			= (1ul<<2ul),
	rsNoVSync			= (1ul<<3ul),
	rsWireframe			= (1ul<<4ul),
	rsAntialias			= (1ul<<6ul),
	rsWarmHZB			= (1ul<<7ul),
	rsNormalize			= (1ul<<8ul),
	rsOcclusion			= (1ul<<10ul),
	rsStatistic			= (1ul<<11ul),
	rsDetails			= (1ul<<12ul),
	rsRefresh60hz		= (1ul<<14ul),
	mtSound				= (1ul<<15ul),
	mtInput				= (1ul<<16ul),
	mtPhysics			= (1ul<<17ul),
	mtNetwork			= (1ul<<18ul)
// 20-32 bit - defined in Editor
};
ENGINE_API extern	u32			psCurrentMode		;
ENGINE_API extern	u32			psCurrentBPP		;
ENGINE_API extern	Flags32		psDeviceFlags		;

#define _game_data_				"$game_data$"
#define _game_textures_			"$game_textures$"
#define _game_levels_			"$game_levels$"
#define _game_sounds_			"$game_sounds$"
#define _game_meshes_			"$game_meshes$"
#define _game_shaders_			"$game_shaders$"

#endif
