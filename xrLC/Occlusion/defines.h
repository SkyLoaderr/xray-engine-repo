#ifndef _DEFINES_H_
#define _DEFINES_H_
//--------------------------------------------------------------------
struct _stats {
	DWORD	dwStartTime;
	DWORD	dwSlots;
	DWORD	dwClipped;
	float	fProgress;
};
extern _stats STAT;
struct _options {
	char	folder[_MAX_PATH];
	bool	bFullVolume;
	bool	bDoubleResolution;
	DWORD	dwVisSize;
	DWORD	dwVisLevels;
};
extern _options MODE;
//--------------------------------------------------------------------
#define MAX_LEVELS		128
#define MAX_LINE		1024

typedef	char		FILE_NAME	[ _MAX_PATH	];
typedef	char		JOIN_NAME	[ 32		];
typedef	char		SHORT_NAME	[ 64		];
typedef char		OBJECT_NAME	[ 128		];


extern ENGINE_API const char * AVI_PATH;
extern ENGINE_API const char * MAPS_PATH;
extern ENGINE_API const char * SOUNDS_PATH;
extern ENGINE_API const char * MESHES_PATH;
extern ENGINE_API const char * CFORMS_PATH;
extern ENGINE_API const char * SHADERS_PATH;
extern ENGINE_API const char * NAME_OF_THE_APP;
extern ENGINE_API const char * CACHE_PATH;

enum NR_RESULT			{
	NR_FREE			= 0,
	NR_MOVE			= 1,
	NR_BUSY			= 2
};

#define	_CHECK(x)				if( !(x) ) _FAIL
#define _BREAK(x)				if( x ) break
#define	_CHECK_HRESULT(x)		if( (x)!=S_OK )_FAIL
#define	_CONFIRM(x)				if( (x) ) _OK
#define _CONTINUE(x)			if( x ) continue
#define _CROP(x,l,h)			if((x)<(l)) (x)=(l);else if((x)>(h)) (x)=(h);
#define _DEFAULT(x,y)			{ if(!(x))(x)=(y); }
#define	_EXIT(x)				if( !(x) )	return
#define	_EXIT_(x)				if( x )	return
#define	_FAIL					return FALSE
#define _FZERO(x)				( fabsf(x)<EPS )
#define _FZERO2(x, y)			( fabsf(x)<(y) )
#define _FTRUNC(x)				if(_FZERO(x)) {(x)=0.0f;}
#define _KEYDOWN(name,key)		( name[key] & 0x80 )
#define	_OK						return TRUE
#define randf()					( float(rand())/float(RAND_MAX) )
#define X_TO_REAL(_X_)			((_X_)+1.f)*float(Device.dwWidth/2)
#define Y_TO_REAL(_Y_)			((_Y_)+1.f)*float(Device.dwHeight/2)

#define MAX_PLAYER_NAME      14
#define MAX_SESSION_NAME     256
extern struct _GUID g_AppGUID;

// ---==( Extended Debugging Support (R) )==---
ENGINE_API void __fastcall _verify(const char *expr, char *file, int line);
#define R_ASSERT(expr)	if (!(expr)) _verify(#expr, __FILE__, __LINE__)
#define R_CHK(expr)		{ HRESULT hr = expr; if (FAILED(hr)) InterpretError(hr, __FILE__, __LINE__); }

#ifdef DEBUG
ENGINE_API void __fastcall _unroll(const char *, const char *);
#define VERIFY(expr) if (!(expr)) _verify(#expr, __FILE__, __LINE__)
#define VERIFY2(expr, info) if (!(expr)) { char buf[128]; sprintf(buf,"%s, %s",#expr,info); _verify(buf, __FILE__, __LINE__); }
#define VERIFY3(expr, info, info2) if (!(expr)) { char buf[128]; sprintf(buf,"%s, %s, %s",#expr,info,info2); _verify(buf, __FILE__, __LINE__); }
#define _E(s) s { static const char *_stack_unwind = #s; try{ VERIFY(this);
#define _EF(s) s { static const char *_stack_unwind = #s; try{
#define _end } catch(...) { _unroll(__FILE__,_stack_unwind); throw; } }
template <class T> T* _P(T* p) {
	VERIFY(p);
	VERIFY(_CrtIsValidHeapPointer(p));
	return p;
}
#define CHK_DX(a)			{ HRESULT hr = a; if (FAILED(hr)) InterpretError(hr, __FILE__, __LINE__); }
#else
#pragma intrinsic (abs, fabs, fmod, sin, cos, tan, asin, acos, atan, sqrt, exp, log, log10, strcpy, strcat)
#define VERIFY(expr)
#define VERIFY2(expr, info)
#define VERIFY3(expr, info, info2)
#define _E(s) s {
#define _EF(s) s {
#define _end }
#define _P(s) s
#define CHK_DX(a)			a
#endif

// 'cause delete(NULL) is valid - there is no sence to check for NULL :)
#define _DELETE(x)			{ delete x;	(x)=NULL; }
#define _DELETEARRAY(x)		{ delete[] x;	(x)=NULL; }
#define _RELEASE(x)			{ if(x) { (x)->Release();       (x)=NULL; } }
#define _FREE(x)			{ if(x) { free(x);              (x)=NULL; } }
#define _SHOW_REF(msg, x)   { VERIFY(x); x->AddRef(); DWORDToFile(msg,x->Release()); }
#define THROW				throw 123456789


// sound
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

// Effects
#define EFF_NONE			0x0000
#define EFF_SKY				0x0001
#define EFF_SKY_FAST		0x0002
#define EFF_STARS			0x0004
#define EFF_CLOUDS			0x0008
#define EFF_SUN				0x0010
#define EFF_FLARES			0x0020

// Device flags
#define	rsFullscreen		0x0001
#define	rsTriplebuffer		0x0002
#define	rsClearBB			0x0004
#define	rsNoVSync			0x0008
#define rsWireframe			0x0010
#define rsAntialias			0x0020
#define rsNormalize			0x0040
#define rsShadows			0x0080

// Sound
ENGINE_API extern DWORD	psSoundFreq		;
ENGINE_API extern DWORD	psSoundModel	;
ENGINE_API extern float	psSoundVMaster	;
ENGINE_API extern float	psSoundVEffects	;
ENGINE_API extern float	psSoundVMusic	;

// textures
#define	dfTextureMono		0x0001
ENGINE_API extern	DWORD   psTextureFormat		;
ENGINE_API extern	DWORD	psTextureMipGen		;
ENGINE_API extern	DWORD	psTextureMipFilter	;
ENGINE_API extern	float	psTextureContrast	;
ENGINE_API extern	float	psTextureLMContrast	;
ENGINE_API extern	float	psTextureLOD		;
ENGINE_API extern	DWORD	psTextureFlags		;

// users
ENGINE_API extern	char	psUserName[64];
ENGINE_API extern	bool	psUserLevelsDone[MAX_LEVELS];

// game
ENGINE_API extern	DWORD	psNetProtocol	;
ENGINE_API extern	char	psName[12]		;

// video
ENGINE_API extern DWORD	psCurrentMode	;
ENGINE_API extern DWORD	psCurrentBPP	;
ENGINE_API extern DWORD	psDeviceFlags	;
ENGINE_API extern DWORD psLightModel	;

#ifdef DEBUG
#define UP_PRIORITY
#define DOWN_PRIORITY
#else
#define UP_PRIORITY		SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS)
#define DOWN_PRIORITY	SetPriorityClass(GetCurrentProcess(),IDLE_PRIORITY_CLASS)
#endif

extern ENGINE_API void InterpretError(HRESULT hr, const char *file, int line);


#endif
