//----------------------------------------------------
// file: stdafx.h
//----------------------------------------------------
#ifndef _INCDEF_STDAFX_H_
#define _INCDEF_STDAFX_H_

#pragma once

// VCL headers
#include <vcl.h>

// Windows headers
#include <windows.h>
#include <mmsystem.h>

// DirectX headers
#include <ddraw.h>
#include <dsound.h>
#include <dinput.h>
#include <dplay.h>
#include <d3d.h>

// Std C++ headers
#include <math.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <process.h>

// C++ STL headers
#include <vector>
#include <list>
#include <deque>
#include <stack>
#include <map>
#include <set>
#include <algorithm>
#include <functional>
using namespace std;

// Visual C++ headers
#include <crtdbg.h>

#define IC				__forceinline

#define _MIN(a,b) (((a)<(b))?(a):(b))
#define _MAX(a,b) (((a)>(b))?(a):(b))

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
__inline int _dup    (int handle)                           { return ::dup(handle);}
__inline float modff(float a, float *b){
	double x,y;
    y = modf(double(a),&x);
    *b = x;
    return float(y);
}
__inline float expf	(float val)                           	{ return ::exp(val);}

namespace std{
__inline float _cpp_min(float a, float b){ return _MIN(a,b);}
__inline float _cpp_max(float a, float b){ return _MAX(a,b);}
}


#define _RELEASE( _Object_ )\
	if( (_Object_) ){\
		(_Object_)->Release();\
		(_Object_) = 0; };

#define MAKE_FOURCC(_0,_1,_2,_3)\
	( (DWORD)(_0) | ((DWORD)(_1)<<8) | ((DWORD)(_2)<<16) | ((DWORD)(_3)<<24) )

void __fastcall _verify(const char *expr, char *file, int line);
#define VERIFY(expr) if (!(expr)) _verify(#expr, __FILE__, __LINE__)
#define VERIFY2(expr, info) if (!(expr)) { char buf[128]; sprintf(buf,"%s, %s",#expr,info); _verify(buf, __FILE__, __LINE__); }
#define R_ASSERT(expr) if (!(expr)) _verify(#expr, __FILE__, __LINE__)
#define R_ASSERT2(expr, info) if (!(expr)) { char buf[128]; sprintf(buf,"%s, %s",#expr,info); _verify(buf, __FILE__, __LINE__); }
#define THROW _verify("ERROR", __FILE__, __LINE__)
#define THROW2(expr) _verify(#expr, __FILE__, __LINE__)
#define NODEFAULT THROW

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
#define atan2f atan2
#define logf log

#define ENGINE_API
#define DLL_API			__declspec(dllimport)

// float redefine
#define _PC_24 PC_24
#define _PC_53 PC_53
#define _PC_64 PC_64
#define _RC_CHOP RC_CHOP
#define _RC_NEAR RC_NEAR

// some user components
#include "shared/dx7todx8.h"
#include "clsid.h"
#include "shared/vector.h"
#include "shared/FixedVector.h"
#include "shared/xr_list.h"
#include "shared/fvf.h"
#include "Log.h"
extern "C" DLL_API LPCSTR InterpretError(HRESULT hr);
#define CDX(_expr_){HRESULT hr=_expr_; if (hr!=DD_OK){char buf[255]; sprintf(buf,"%s\n\nD3D Error: %s",#_expr_,InterpretError(hr)); _verify(buf, __FILE__, __LINE__);}}

#define X_TO_REAL(_X_)			((_X_)+1.f)*float(float(UI->GetRenderWidth())/2.f)
#define Y_TO_REAL(_Y_)			((_Y_)+1.f)*float(float(UI->GetRenderHeight())/2.f)

#define MAX_FOLDER_NAME    255
#define MAX_OBJ_NAME       64
#define MAX_OBJCLS_NAME    64
#define MAX_CLASS_SCRIPT   4096
#define MAX_LINK_NAME      64
#define MAX_LTX_ADD        16384
#define MAX_ADD_FILES_TEXT 1024

#define DEFINE_SVECTOR(type,sz,lst,it)\
	typedef svector<type,sz> lst;\
	typedef lst::iterator it;

DEFINE_VECTOR(bool,BOOLVec,BOOLIt);
DEFINE_VECTOR(BYTE,BYTEVec,BYTEIt);
DEFINE_VECTOR(WORD,WORDVec,WORDIt);
DEFINE_VECTOR(DWORD,DWORDVec,DWORDIt);
DEFINE_VECTOR(int,INTVec,INTIt);
DEFINE_VECTOR(float,FloatVec,FloatIt);
DEFINE_VECTOR(Fplane,PlaneVec,PlaneIt);      
DEFINE_VECTOR(Fvector2,Fvector2Vec,Fvector2It);
DEFINE_VECTOR(Fvector,FvectorVec,FvectorIt);
DEFINE_VECTOR(FVF::L,FLvertexVec,FLvertexIt);
DEFINE_VECTOR(FVF::LIT,FLITvertexVec,FLITvertexIt);
DEFINE_VECTOR(AnsiString,AStringVec,AStringIt);

extern ENGINE_API Fmatrix precalc_identity;;
extern ENGINE_API Fbox box_identity;
extern bool g_ErrorMode;

#endif /*_INCDEF_STDAFX_H_*/





