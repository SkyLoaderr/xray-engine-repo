//----------------------------------------------------
// file: stdafx.h
//----------------------------------------------------
#ifndef __INCDEF_STDAFX_H_
#define __INCDEF_STDAFX_H_

#pragma once

#define WIN32_LEAN_AND_MEAN			// Exclude rarely-used stuff from Windows headers
#define STRICT						// Enable strict syntax
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOCLIPBOARD
#define NODRAWTEXT
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOSERVICE
#define NOCOMM
#define NOHELP
#define NOPROFILER
#define NOMCX

#include <windows.h>

#include <windowsx.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include <tchar.h>
#include <commctrl.h>
#include <direct.h>
#include <commdlg.h>

#include <malloc.h>
//#ifdef DEBUG
#include <crtdbg.h>
//#endif
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <process.h>
#include <assert.h>

#include "d3d8.h"
#include "d3dx8.h"

#ifndef _DEBUG
#pragma inline_depth	( 255 )
#pragma inline_recursion( on  )
#pragma intrinsic		(abs, fabs, fmod, sin, cos, tan, asin, acos, atan, sqrt, exp, log, log10, strcpy, strcat)
#endif

// Warnings
#pragma warning (disable : 4786 )		// too long names
#pragma warning (disable : 4251 )		// object needs DLL interface

// stl
#include <algorithm>
#include <numeric>
#include <limits>
#include <vector>
#include <stack>
#include <list>
#include <set>
#include <map>
#include <deque>
#include <string>

#define _MIN(a,b)		(a)<(b)?(a):(b)
#define _MAX(a,b)		(a)>(b)?(a):(b)
template <class T>
T min(T a, T b) { return _MIN(a,b); }
template <class T>
T max(T a, T b) { return _MAX(a,b); }
using std::string;
#undef _MIN
#undef _MAX

#include "Max.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "phyexp.h"
#include "bipexp.h"
#include "stdmat.h"
#include "UTILAPI.H"

#define IC				__forceinline

// iseful macros
// MSC names for functions
// some user components
#define _RELEASE( _Object_ )\
	if( (_Object_) ){\
		(_Object_)->Release();\
		(_Object_) = 0; };

#define MAKE_FOURCC(_0,_1,_2,_3)\
	( (DWORD)(_0) | ((DWORD)(_1)<<8) | ((DWORD)(_2)<<16) | ((DWORD)(_3)<<24) )

extern "C" __declspec(dllimport) HRESULT InterpretError(HRESULT hr);
#define CDX( _expr_ )\
	if( DD_OK != (_expr_) ){\
		Log->Msg( mtError, "D3D: error at %d in '%d' !", __LINE__, #_expr_ );\
        InterpretError(_expr_);\
     }

void __fastcall _verify(const char *expr, char *file, int line);
#define VERIFY(expr) if (!(expr)) _verify(#expr, __FILE__, __LINE__)
#define R_ASSERT(a) VERIFY(a)

#define _FREE(x)		{ if(x) { free(x); (x)=NULL; } }
#define _DELETE(a)      {delete(a); (a)=NULL;}
#define _DELETEARRAY(a) {delete[](a); (a)=NULL;}
#define fabsf(a) fabs(a)
#define sinf(a) sin(a)
#define asinf(a) asin(a)
#define cosf(a) cos(a)
#define acosf(a) acos(a)
#define tanf(a) tan(a)
#define atanf(a) atan(a)
#define sqrtf(a) sqrt(a)
#define __forceinline __inline
#define floorf floor

#define ENGINE_API

// some user components
// Our headers
#define NO_XR_LIGHT
#define NO_XR_MATERIAL
#define NO_XR_3DBUFFER
#define NO_XR_3DLISTENER
#define NO_XR_VERTEX
//#define NO_XR_COLOR
#define NODEFAULT R_ASSERT(0)
#include "vector.h"
#include "fixedvector.h"
#include "NetDeviceLog.h"
#include "FS.h"


#define MAX_FOLDER_NAME    255
#define MAX_OBJ_NAME       64
#define MAX_OBJCLS_NAME    64
#define MAX_CLASS_SCRIPT   4096
#define MAX_LINK_NAME      64
#define MAX_LTX_ADD        16384
#define MAX_ADD_FILES_TEXT 1024

#define DEFINE_MAP(key,type,lst,it)\
	typedef std::map<key,type> lst;\
	typedef lst::iterator it;
#define DEFINE_VECTOR(type,lst,it)\
	typedef std::vector<type> lst;\
	typedef lst::iterator it;
#define DEFINE_SVECTOR(type,sz,lst,it)\
	typedef svector<type,sz> lst;\
	typedef lst::iterator it;

DEFINE_VECTOR(DWORD,DWORDList,DWORDIt);
DEFINE_VECTOR(int,INTList,INTIt);
DEFINE_VECTOR(WORD,WORDList,WORDIt);
DEFINE_VECTOR(Fplane,PlaneList,PlaneIt);
DEFINE_VECTOR(Fvector,FvectorList,FvectorIt);
DEFINE_VECTOR(Fvector2,Fvector2List,Fvector2It);
DEFINE_VECTOR(bool,BOOLList,BOOLIt);
DEFINE_VECTOR(float,FloatList,FloatIt);
DEFINE_VECTOR(string,AStringList,AStringIt);

extern ENGINE_API Fmatrix precalc_identity;;
extern bool g_ErrorMode;

// CLASS ID type
typedef unsigned __int64	CLASS_ID;
#define MK_CLSID(a,b,c,d,e,f,g,h) \
	CLASS_ID(	((CLASS_ID(a)<<CLASS_ID(24))|(CLASS_ID(b)<<CLASS_ID(16))|(CLASS_ID(c)<<CLASS_ID(8))|(CLASS_ID(d)))<<CLASS_ID(32) | \
				((CLASS_ID(e)<<CLASS_ID(24))|(CLASS_ID(f)<<CLASS_ID(16))|(CLASS_ID(g)<<CLASS_ID(8))|(CLASS_ID(h))) )

#endif /*_INCDEF_STDAFX_H_*/





