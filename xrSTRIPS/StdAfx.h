// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__2BEBA17E_365A_440A_9494_24541E067502__INCLUDED_)
#define AFX_STDAFX_H__2BEBA17E_365A_440A_9494_24541E067502__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define STRICT
#include <windows.h>
#include <assert.h>
#include <stdlib.h>
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

#pragma warning( disable : 4786 )  

#include <vector>
#include <list>
#include <set>

#ifdef  __cplusplus
extern "C" {
#endif

_CRTIMP void __cdecl _assert(void *, void *, unsigned);

#ifdef  __cplusplus
}
#endif

#define R_ASSERT(exp)	(void)( (exp) || (_assert(#exp, __FILE__, __LINE__), 0) )
#define VERIFY(exp)		assert(exp)

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__2BEBA17E_365A_440A_9494_24541E067502__INCLUDED_)
