#pragma once

#define STRICT
#include <Windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <math.h>
#include <D3DX9.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFile.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "Util.h"

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
#pragma warning (disable : 4305 )

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

