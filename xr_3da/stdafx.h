// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:48:12 , by user : Oles , from computer : OLES
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
// Third generation by Oles.

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#pragma once
#define WIN32_LEAN_AND_MEAN			// Exclude rarely-used stuff from Windows headers
#define STRICT						// Enable strict syntax
#define IDIRECTPLAY2_OR_GREATER

// windows.h
#define _WIN32_WINNT 0x0400
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NODRAWTEXT
#define NOMEMMGR
#define NOMETAFILE
#define NOSERVICE
#define NOCOMM
#define NOHELP
#define NOPROFILER
#define NOMCX
#define NOMINMAX
#define DOSWIN32
#define _WIN32_DCOM

#include <windows.h>
#include <windowsx.h>

// mmsystem.h
#define MMNOSOUND
#define MMNOMIDI
#define MMNOAUX
#define MMNOMIXER
#define MMNOJOY

#include <mmsystem.h>

// mmreg.h
#define NOMMIDS
#define NONEWRIFF
#define NOJPEGDIB
#define NONEWIC
#define NOBITMAP

#include <mmreg.h>

//#include <winbase.h>
//#include <winnls.h>

#define DIRECTSOUND_VERSION 0x0700
#define DIRECTINPUT_VERSION 0x0700

#include <d3d8.h>
#include <d3dx8.h>
#include <dinput.h>
#include <dsound.h>
#include <dplay8.h>

#include <malloc.h>
#ifdef DEBUG
#include <crtdbg.h>
#endif
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

#ifndef DEBUG
#pragma inline_depth	( 254 )
#pragma inline_recursion( on )
#pragma intrinsic		(abs, fabs, fmod, sin, cos, tan, asin, acos, atan, sqrt, exp, log, log10, strcpy, strcat)
#define inline			__forceinline
#define _inline			__forceinline
#define __inline		__forceinline
#define IC				__forceinline
#else
#define IC				__forceinline
#endif

// Warnings
#pragma warning (disable : 4786 )		// too long names
#pragma warning (disable : 4503 )		// decorated name length exceeded
#pragma warning (disable : 4251 )		// object needs DLL interface
#pragma warning (disable : 4201 )		// nonstandard extension used : nameless struct/union
#pragma warning (disable : 4100 )		// unreferenced formal parameter
#pragma warning (disable : 4127 )		// conditional expression is constant
#pragma warning (disable : 4324 )		// structure was padded due to __declspec(align())

// stl
#define _STLP_NO_EXCEPTIONS 
#include <algorithm>
#include <limits>
#include <vector>
#include <stack>
#include <list>
#include <set>
#include <map>
using namespace std;

// Property-set support
#define PropertyG(a)	__declspec( property( get=a ) )
#define PropertyP(a)	__declspec( property( put=a ) )
#define PropertyGP(a,b)	__declspec( property( get=a, put=b ) )
#define ALIGN(a)		__declspec(align(a))

// you must define ENGINE_BUILD then building the engine itself
// and not define it if you are about to build DLL
#ifndef NO_ENGINE_API
	#ifdef	ENGINE_BUILD
		#define DLL_API			__declspec(dllimport)
		#define ENGINE_API		__declspec(dllexport)
	#else
		#define DLL_API			__declspec(dllexport)
		#define ENGINE_API		__declspec(dllimport)
	#endif
#else
	#define ENGINE_API
	#define DLL_API
#endif // NO_ENGINE_API

// Our headers
#define VERIFY assert
#include "vector.h"
#undef  VERIFY
#include "engine.h"
#ifndef NO_MMGR
#include "fmemorymanager.h"
#endif
#include "defines.h"
#include "pure.h"
#ifndef NO_XRLOG
#include "log.h"
#endif
#include "device.h"
#include "fs.h"
#include "properties.h"
#include "xrXRC.h"

// TODO: reference additional headers your program requires here

#pragma comment( lib, "x:\\game\\xrCDB.lib"	)

#pragma comment( lib, "imagehlp.lib"	)
#pragma comment( lib, "winmm.lib"		)

#pragma comment( lib, "d3d8.lib"		)
#pragma comment( lib, "dxerr8.lib"		)

#pragma comment( lib, "d3dx8.lib"		)

#pragma comment( lib, "dsound.lib"		)
#pragma comment( lib, "dinput.lib"		)
#pragma comment( lib, "dxguid.lib"		)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
