// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__609C96C7_EF92_4B6B_9398_C8AF46E6BA4E__INCLUDED_)
#define AFX_STDAFX_H__609C96C7_EF92_4B6B_9398_C8AF46E6BA4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <assert.h>
#include <direct.h>
#include <d3d8.h>

#include "limits"
#include "list"
#include "set"
#include "map"
#include "stack"
#include "algorithm"
using namespace std;

#undef min
#undef max
#define ENGINE_API 
#define IC __forceinline
#define NO_XR_LIGHT
#define NO_XR_MATERIAL
#define NO_XR_3DBUFFER
#define VERIFY(a)		assert(a)
#define R_ASSERT(a)		assert(a)
#define R_ASSERT2(a,b)	assert(a)
#define Msg printf
#define NODEFAULT
#define xr_malloc	malloc
#define xr_realloc	realloc
#define xr_free		free
#define xr_strdup	strdup
#define _FREE(a) { if (a) xr_free(a); }
#include "vector.h"

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__609C96C7_EF92_4B6B_9398_C8AF46E6BA4E__INCLUDED_)
