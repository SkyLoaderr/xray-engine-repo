// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XRCORE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XRCORE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef XR_CORE_H
#define XR_CORE_H

#pragma once
#define WIN32_LEAN_AND_MEAN			// Exclude rarely-used stuff from Windows headers
#define STRICT						// Enable strict syntax
#define IDIRECTPLAY2_OR_GREATER

// windows.h
#define _WIN32_WINNT 0x0500        
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

// #include <winbase.h>
// #include <winnls.h>
// #include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <process.h>
#include <assert.h>
#include <typeinfo.h>

#ifdef __BORLANDC__
	#define MODULE_NAME 	"xrCoreB.dll"
    #ifndef DEBUG
    	#pragma inline_depth	( 254 )
	    #pragma inline_recursion( on )
	    #pragma intrinsic		(abs, fabs, fmod, sin, cos, tan, asin, acos, atan, sqrt, exp, log, log10, strcpy, strcat)
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

// Warnings
#pragma warning (disable : 4786 )		// too long names
#pragma warning (disable : 4503 )		// decorated name length exceeded
#pragma warning (disable : 4251 )		// object needs DLL interface
#pragma warning (disable : 4201 )		// nonstandard extension used : nameless struct/union
#pragma warning (disable : 4100 )		// unreferenced formal parameter
#pragma warning (disable : 4127 )		// conditional expression is constant
#pragma warning (disable : 4324 )		// structure was padded due to __declspec(align())
#pragma warning (disable : 4714 )		// 'function' marked as __forceinline not inlined

// Our headers
#ifdef XRCORE_EXPORTS
#define XRCORE_API __declspec(dllexport)
#else
#define XRCORE_API __declspec(dllimport)
#endif


#define VERIFY assert

#include "vector.h"
#undef  VERIFY
#include "xrSyncronize.h"
#include "xrMemory.h"
#include "xrDebug.h"

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
	string512	Params;
public:
	void		_initialize	(const char* ApplicationName);
	void		_destroy	();
};
extern XRCORE_API xrCore Core;

#endif
