// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D0627F69_2DAD_4F2B_B0D3_9A1B789B2D06__INCLUDED_)
#define AFX_STDAFX_H__D0627F69_2DAD_4F2B_B0D3_9A1B789B2D06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN			// Exclude rarely-used stuff from Windows headers
#define STRICT						// Enable strict syntax
#define IDIRECTPLAY2_OR_GREATER

#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOCLIPBOARD
#define NODRAWTEXT
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOSERVICE
#define NOCOMM
#define NOHELP
#define NOPROFILER
#define NOMCX

#include <windows.h>
#include <mmsystem.h>
#include <winnls.h>

#include <d3d8.h>
#include <d3dx8.h>
#include <dplay8.h>

#include <stdio.h>
#include <math.h>

#ifndef _DEBUG
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
using namespace std;

// defines
extern void _error(HRESULT hr,LPCSTR file,int line);
#define CHK_DX(a)			{ HRESULT hr = a; if (FAILED(hr)) _error(hr, __FILE__, __LINE__); }

#define ENGINE_API
#define VERIFY(a)
#define R_ASSERT(a)
#define _RELEASE(a) { if (a) { a->Release(); a=0; } }
#define NODEFAULT
#define Msg	printf

#define NO_XR_COLOR
#define NO_XR_LIGHT
#define NO_XR_MATERIAL
#define NO_XR_3DBUFFER
#define NO_XR_3DLISTENER

#include "vector.h"


// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D0627F69_2DAD_4F2B_B0D3_9A1B789B2D06__INCLUDED_)
