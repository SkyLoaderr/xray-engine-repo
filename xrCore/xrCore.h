// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XRCORE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XRCORE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef xrCoreH
#define xrCoreH

#pragma once
#define WIN32_LEAN_AND_MEAN			// Exclude rarely-used stuff from Windows headers
#define STRICT						// Enable strict syntax
#define IDIRECTPLAY2_OR_GREATER

// windows.h
#define _WIN32_WINNT 0x0500        

#ifdef __BORLANDC__
	#include <vcl.h>
	#include <mmsystem.h>
	#include <stdint.h>
#endif

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

#ifndef __BORLANDC__
	#include <windowsx.h>
#endif

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


// stl-config
// *** disable exceptions for both STLport and VC7.1 STL
#define _STLP_NO_EXCEPTIONS	1

// *** try to minimize code bloat of STLport
#ifdef __BORLANDC__
#else
#ifdef XRCORE_EXPORTS				// no exceptions, export allocator and common stuff
#define _STLP_DESIGNATED_DLL 1
#define _STLP_USE_DECLSPEC 1
#else
#define _STLP_USE_DECLSPEC 1	// no exceptions, import allocator and common stuff
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <process.h>
#include <assert.h>
#include <typeinfo.h>

#ifndef DEBUG
	#ifdef _DEBUG
    	#define DEBUG
    #endif
	#ifdef MIXED
    	#define DEBUG
    #endif
#endif

#ifdef __BORLANDC__
	#include <utime.h>
	#define _utimbuf utimbuf
	#define MODULE_NAME 		"xrCoreB.dll"
    #ifndef DEBUG
    	#pragma inline_depth	( 254 )
	    #pragma inline_recursion( on )
//	    #pragma intrinsic		(abs, fabs, fmod, sin, cos, tan, asin, acos, atan, sqrt, exp, log, log10, strcpy, strcat)
	    #define __forceinline	inline
	    #define _inline			inline
	    #define __inline		inline
	    #define IC				inline
    #else
	    #define __forceinline	inline
	    #define IC				inline
    #endif
    // function redefinition
    #define fabsf(a) fabs(a)
    #define sinf(a) sin(a)
    #define asinf(a) asin(a)
    #define cosf(a) cos(a)
    #define acosf(a) acos(a)
    #define tanf(a) tan(a)
    #define atanf(a) atan(a)
    #define sqrtf(a) sqrt(a)
    #define expf(a) ::exp(a)
    #define floorf floor
    #define atan2f atan2
    #define logf log
	// float redefine
	#define _PC_24 PC_24
	#define _PC_53 PC_53
	#define _PC_64 PC_64
	#define _RC_CHOP RC_CHOP
	#define _RC_NEAR RC_NEAR
#else
	#include <sys\utime.h>
	#define MODULE_NAME 	"xrCore.dll"
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
#endif

// Warnings
#pragma warning (disable : 4251 )		// object needs DLL interface
#pragma warning (disable : 4201 )		// nonstandard extension used : nameless struct/union
#pragma warning (disable : 4100 )		// unreferenced formal parameter
#pragma warning (disable : 4127 )		// conditional expression is constant
#pragma warning (disable : 4324 )		// structure was padded due to __declspec(align())
#pragma warning (disable : 4714 )		// 'function' marked as __forceinline not inlined
#pragma warning (disable : 4530 )		// C++ exception handler used, but unwind semantics are not enabled
#pragma warning (disable : 4345 )

// stl
#include <algorithm>
#include <limits>
#include <vector>
#include <stack>
#include <list>
#include <set>
#include <map>

// Our headers
#ifdef XRCORE_EXPORTS
#define XRCORE_API __declspec(dllexport)
#else
#define XRCORE_API __declspec(dllimport)
#endif

#define VERIFY assert
#include "vector.h"
#undef  VERIFY

#include "clsid.h"
#include "xrSyncronize.h"
#include "xrMemory.h"
#include "xrDebug.h"

#include "_stl_extensions.h"
#include "xrstring.h"
#include "rt_compressor.h"

#include "FS.h"
#include "log.h"
#include "xr_trims.h"
#include "xr_ini.h"
#include "LocatorAPI.h"
#include "FileSystem.h"
#include "FTimer.h"

template <class T>
class destructor
{
	T* ptr;
public:
	destructor(T* p)	{ ptr=p;			}
	~destructor()		{ xr_delete(ptr);	}
	IC T& operator() ()
	{	return *ptr; }
};

// ********************************************** The Core definition

class XRCORE_API xrCore 
{
public:
	string64	ApplicationName;
	string64	UserName;
	string64	CompName;
	string512	Params;
public:
	void		_initialize	(LPCSTR ApplicationName, LogCallback cb=0, LPCSTR root_path=0);
	void		_destroy	();
};
extern XRCORE_API xrCore Core;

#endif
