// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__81632403_DFD8_4A42_A4D3_0AFDD8EA0D25__INCLUDED_)
#define AFX_STDAFX_H__81632403_DFD8_4A42_A4D3_0AFDD8EA0D25__INCLUDED_

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

#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include <tchar.h>
#include <commctrl.h>

#include <d3d8.h>
#include <d3dx8.h>
#include <dx7todx8.h>

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

#ifndef DEBUG
#pragma inline_depth	( 255 )
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
#pragma warning (disable : 4251 )		// object needs DLL interface

#define ENGINE_API

// stl
#include <algorithm>
#include <numeric>
#include <vector>
#include <stack>
#include <list>
#include <set>
#include <map>
#include <deque>
#include <string>
#include <limits>
using namespace std;

#define RGBA_GETALPHA(rgb)      ((rgb) >> 24)
#define RGBA_GETRED(rgb)        (((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      (((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       ((rgb) & 0xff)

// Our headers
#define NO_XR_MATERIAL
#define NO_XR_3DBUFFER
#define NO_XR_3DLISTENER
#define NO_XR_VERTEX
#define ENGINE_API
#include "clsid.h"
#include "debugkernel.h"
#include "defines.h"
#include "vector.h"
#include "log.h"
#include "fs.h"
#include "xr_list.h"
#include "fixedvector.h"

#include "communicate.h"
extern b_params	g_params;

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__81632403_DFD8_4A42_A4D3_0AFDD8EA0D25__INCLUDED_)
