// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7AB149AA_1804_4D7C_AA62_8E1E3B0C8F0B__INCLUDED_)
#define AFX_STDAFX_H__7AB149AA_1804_4D7C_AA62_8E1E3B0C8F0B__INCLUDED_
#pragma once

#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "limits"
#include "list"

#undef min
#undef max
#define ENGINE_API 
#define IC __forceinline
#define	NO_XR_COLOR
#define NO_XR_LIGHT
#define NO_XR_MATERIAL
#define NO_XR_3DBUFFER
#define VERIFY(a)
#define R_ASSERT(a)
#define Msg printf
struct D3DVECTOR;
struct D3DMATRIX;
#define NODEFAULT
#include "vector.h"
// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7AB149AA_1804_4D7C_AA62_8E1E3B0C8F0B__INCLUDED_)
