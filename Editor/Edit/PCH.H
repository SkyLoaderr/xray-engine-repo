//----------------------------------------------------
// file: Pch.h
//----------------------------------------------------


#ifndef _INCDEF_PCH_H_
#define _INCDEF_PCH_H_

#pragma once

// VCL headers
#include <vcl.h>
// Windows headers
#include <windows.h>
//#include <commctrl.h>
//#include <commdlg.h>
#include <mmsystem.h>
//#include <shlobj.h>
//#include <vfw.h>

// DirectX headers
#include <ddraw.h>
#include <dsound.h>
#include <dinput.h>
#include <dplay.h>
#include <d3d.h>

// OpenGL headers
//#include <gl\gl.h>
//#include <gl\glu.h>

// Visual C++ headers
#include <crtdbg.h>

// Std C++ headers
#include <math.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <process.h>

// C++ STL headers
#pragma warning(disable:4786)
#include <vector>
#include <list>
#include <deque>
#include <algorithm>
using namespace std;

#define _MIN(a,b) (((a)<(b))?(a):(b))
#define _MAX(a,b) (((a)>(b))?(a):(b))


// iseful macros
/* MSC names for functions */
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

#define RND1() ( (rand()/(float)RAND_MAX) )
#define RND()  ( ((RND1() - 0.5f) * 2.f) )

#define SAFE_DELETE( _Object_ )\
	if( (_Object_) ){\
		delete (_Object_);\
		(_Object_) = 0; };

#define SAFE_DELETE_A( _Object_ )\
	if( (_Object_) ){\
		delete [] (_Object_);\
		(_Object_) = 0; };

#define SAFE_RELEASE( _Object_ )\
	if( (_Object_) ){\
		(_Object_)->Release();\
		(_Object_) = 0; };

#define MAKE_FOURCC(_0,_1,_2,_3)\
	( (DWORD)(_0) | ((DWORD)(_1)<<8) | ((DWORD)(_2)<<16) | ((DWORD)(_3)<<24) )

extern "C" __declspec(dllimport) HRESULT WINAPI InterpretError(HRESULT hr);

#define CDX( _expr_ )\
	if( DD_OK != (_expr_) ){\
		Log.Msg( "D3D: error at %d in '%d' !", __LINE__, #_expr_ );\
        InterpretError(_expr_);\
     }

// some types
class SSTR{
public:
	char filename[MAX_PATH];
	SSTR(){filename[0]=0;};
	SSTR(const SSTR& source){strcpy(filename,source.filename);};
	SSTR(char *source){strcpy(filename,source);};
	~SSTR(){}; };

typedef std::vector<SSTR> SSTRLIST;

#define _DELETE(a)      delete(a); (a)=NULL;
#define _DELETEARRAY(a) delete[](a); (a)=NULL;
#define VERIFY(a) _ASSERTE(a)
#define fabsf(a) fabs(a)
#define sinf(a) sin(a)
#define cosf(a) cos(a)
#define sqrtf(a) sqrt(a)
#define __forceinline __inline
#define acosf acos
#define floorf floor
// some user components
#include "vector.h"
#include "IMath.h"
#include "Log.h"
#include "xr_list.h"

#define MAX_FOLDER_NAME    255
#define MAX_OBJ_NAME       64
#define MAX_OBJCLS_NAME    64
#define MAX_CLASS_SCRIPT   4096
#define MAX_LINK_NAME      64
#define MAX_LTX_ADD        16384
#define MAX_ADD_FILES_TEXT 1024

#endif /*_INCDEF_PCH_H_*/




