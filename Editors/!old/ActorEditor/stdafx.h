//----------------------------------------------------
// file: stdafx.h
//----------------------------------------------------
#ifndef stdafxH
#define stdafxH

#pragma once

#pragma warn -pck

#define sqrtf(a) sqrt(a)

#define smart_cast dynamic_cast

#ifndef O_SEQUENTIAL
#define O_SEQUENTIAL 0
#endif

#define DIRECTINPUT_VERSION 0x0700

#define         R_R1    1
#define         R_R2    2
#define         RENDER  R_R1

// Std C++ headers
#include <fastmath.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <process.h>
#include <utime.h>

// iseful macros
// MSC names for functions
#ifdef _eof
#undef _eof
#endif
__inline int _eof	(int _a)   		                        { return ::eof(_a); }
#ifdef _access
#undef _access
#endif
__inline int _access(const char *_a, int _b)                { return ::access(_a,_b); }
#ifdef _lseek
#undef _lseek
#endif
__inline long _lseek(int handle, long offset, int fromwhere){ return ::lseek(handle, offset, fromwhere);}
#ifdef _dup
#undef _dup
#endif
#define fmodf fmod
__inline int _dup    (int handle)                           { return ::dup(handle);}
__inline float modff(float a, float *b){
	double x,y;
    y = modf(double(a),&x);
    *b = x;
    return float(y);
}
__inline float expf	(float val)                           	{ return ::exp(val);}


#ifdef	_ECOREB
    #define ECORE_API		__declspec(dllexport)
    #define ENGINE_API		__declspec(dllexport)
#else
    #define ECORE_API		__declspec(dllimport)
    #define ENGINE_API		__declspec(dllimport)
#endif

#define DLL_API			__declspec(dllimport)
#define PropertyGP(a,b)	__declspec( property( get=a, put=b ) )
#define THROW			Debug.fatal("THROW");
#define THROW2(a)		Debug.fatal(a);

#define clMsg 			Msg

// core
#include <xrCore.h>

#ifdef _EDITOR
	class PropValue;
	class PropItem;
	DEFINE_VECTOR(PropItem*,PropItemVec,PropItemIt);

	class ListItem;
	DEFINE_VECTOR(ListItem*,ListItemsVec,ListItemsIt);
#endif

#include <xrCDB.h>
#include <Sound.h>
#include <PSystem.h>

// DirectX headers
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <dsound.h>

// some user components
#include "engine\fmesh.h"
#include "engine\_d3d_extensions.h"

#include "D3DX_Wrapper.h"
#include "ETools.h"

DEFINE_VECTOR		(AnsiString,AStringVec,AStringIt);
DEFINE_VECTOR		(AnsiString*,LPAStringVec,LPAStringIt);

#include "xrEProps.h"
#include "Log.h"
#include "editor\engine.h"
#include "defines.h"

struct str_pred : public std::binary_function<char*, char*, bool>
{
    IC bool operator()(LPCSTR x, LPCSTR y) const
    {	return strcmp(x,y)<0;	}
};
struct astr_pred : public std::binary_function<const AnsiString&, const AnsiString&, bool>
{
    IC bool operator()(const AnsiString& x, const AnsiString& y) const
    {	return x<y;	}
};

#ifdef _EDITOR
	#include "editor\device.h"
	#include "properties.h"
	#include "render.h"
	DEFINE_VECTOR(FVF::L,FLvertexVec,FLvertexIt);
	DEFINE_VECTOR(FVF::TL,FTLvertexVec,FTLvertexIt);
	DEFINE_VECTOR(FVF::LIT,FLITvertexVec,FLITvertexIt);
	DEFINE_VECTOR(shared_str,RStrVec,RStrVecIt);

	#include "xrXRC.h"

	#include "CustomObject.h"
	#include "EditorPreferences.h"
#endif

#ifdef _LEVEL_EDITOR                
	#include "net_utils.h"
#endif

#define INI_NAME(buf) 		{FS.update_path(buf,"$local_root$",EFS.ChangeFileExt(UI->EditorName(),".ini").c_str());}
#define DEFINE_INI(storage)	{std::string buf; INI_NAME(buf); storage->IniFileName=buf.c_str();}
#define NONE_CAPTION "<none>"
#define MULTIPLESEL_CAPTION "<multiple selection>"

// external dependencies

#ifndef _ECOREB
#	pragma comment(lib,"xrECoreB.lib")
#endif

#pragma comment(lib,"xrEPropsB.lib")

#pragma comment(lib,"xrCoreB.lib")
#pragma comment(lib,"xrSoundB.lib")
#pragma comment(lib,"xrCDBB.lib")

#pragma comment(lib,"dinputB.lib")
#pragma comment(lib,"freeimageB.lib")
//#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"dxtB.lib")
#pragma comment(lib,"xrProgressiveB.lib")
#pragma comment(lib,"MagicFMDLLB.lib")
#pragma comment(lib,"EToolsB.lib")
#pragma comment(lib,"LWOB.lib")
#pragma comment(lib,"xrHemisphereB.lib")
#pragma comment(lib,"oggencB.lib")

// path definition
#define _server_root_		"$server_root$"
#define _server_data_root_	"$server_data_root$"
#define _local_root_		"$local_root$"
#define _import_			"$import$"
#define _sounds_			"$sounds$"
#define _textures_			"$textures$"
#define _objects_			"$objects$"
#define _maps_				"$maps$"
#define _temp_				"$temp$"
#define _omotion_			"$omotion$"
#define _omotions_			"$omotions$"
#define _smotion_			"$smotion$"
#define _detail_objects_	"$detail_objects$"
#endif

#define		TEX_POINT_ATT	"internal\\internal_light_attpoint"
#define		TEX_SPOT_ATT	"internal\\internal_light_attclip"

#pragma hdrstop

