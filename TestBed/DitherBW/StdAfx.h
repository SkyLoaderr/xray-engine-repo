// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F63AD4D1_88FE_49B6_A6CB_1F7DEC4FF723__INCLUDED_)
#define AFX_STDAFX_H__F63AD4D1_88FE_49B6_A6CB_1F7DEC4FF723__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <stdio.h>
#include <malloc.h>
#include <windows.h>
#include <math.h>

#define ENGINE_API
#define _FREE(a) if (a) { free(a); a=0; }
#define R_ASSERT(a)
#define IC __forceinline
#include "image.h"


// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F63AD4D1_88FE_49B6_A6CB_1F7DEC4FF723__INCLUDED_)
