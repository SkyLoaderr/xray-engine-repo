//----------------------------------------------------
// file: Pch.h
//----------------------------------------------------


#ifndef _INCDEF_PCH_H_
#define _INCDEF_PCH_H_

#pragma once
#pragma warning (disable: 4786)

#define IC				__forceinline

#define VERIFY _ASSERTE
// Windows headers
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <mmsystem.h>
#include <shlobj.h>
#include <vfw.h>

// DirectX headers
#include <ddraw.h>
#include <dsound.h>
#include <dinput.h>
#include <dplay.h>
#include <d3d.h>

// OpenGL headers
#include <gl\gl.h>
#include <gl\glu.h>

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
#include <vector>
#include <list>
#include <deque>
#include <algorithm>
#include <functional>

#include "vector.h"

#include "max.h"
#include "iparamm2.h"
#include "utilapi.h"
#include "stdmat.h"
#include "triobj.h"
// iseful macros
using namespace std;

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

#define CDX( _expr_ )\
	if( DD_OK != (_expr_) )\
		NConsole.print( "D3D: error at %d in '%d' !", __LINE__, #_expr_ );

// some types
class SSTR{
public:
	char filename[MAX_PATH];
	SSTR(){filename[0]=0;};
	SSTR(const SSTR& source){strcpy(filename,source.filename);};
	SSTR(char *source){strcpy(filename,source);};
	~SSTR(){}; };

typedef std::vector<SSTR> SSTRLIST;

#define DEFINE_MAP(key,type,lst,it)\
	typedef map<key,type> lst;\
	typedef lst::iterator it;
#define DEFINE_VECTOR(type,lst,it)\
	typedef vector<type> lst;\
	typedef lst::iterator it;

DEFINE_VECTOR(DWORD,DWORDList,DWORDIt);
DEFINE_VECTOR(int,INTList,INTIt);
DEFINE_VECTOR(WORD,WORDList,WORDIt);
DEFINE_VECTOR(Fplane,PlaneList,PlaneIt);
DEFINE_VECTOR(Fvector,FvectorList,FvectorIt);
DEFINE_VECTOR(Fvector2,Fvector2List,Fvector2It);
DEFINE_VECTOR(Fvertex,FvertexList,FvertexIt);
DEFINE_VECTOR(bool,BOOLList,BOOLIt);
DEFINE_VECTOR(float,FloatList,FloatIt);
DEFINE_VECTOR(String,AStringList,AStringIt);

#define MAX_FOLDER_NAME    255
#define MAX_OBJ_NAME       64
#define MAX_OBJCLS_NAME    64
#define MAX_CLASS_SCRIPT   4096
#define MAX_LINK_NAME      64
#define MAX_LTX_ADD        16384
#define MAX_ADD_FILES_TEXT 1024

#endif /*_INCDEF_PCH_H_*/




