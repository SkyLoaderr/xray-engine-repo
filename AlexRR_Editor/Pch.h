//----------------------------------------------------
// file: Pch.h
//----------------------------------------------------


#ifndef _INCDEF_PCH_H_
#define _INCDEF_PCH_H_

#pragma once


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
#pragma warning(disable:4786)
#include <vector>
#include <list>
#include <deque>
#include <algorithm>
using namespace std;

// iseful macros

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


// some user components
#include "IMath.h"
#include "XScript.h"
#include "NetDeviceLog.h"


#endif /*_INCDEF_PCH_H_*/




