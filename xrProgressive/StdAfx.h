// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__E0B7368F_9D0A_4316_93C4_7E41943BC778__INCLUDED_)
#define AFX_STDAFX_H__E0B7368F_9D0A_4316_93C4_7E41943BC778__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <math.h>
#include <assert.h>
#include <crtdbg.h>

#ifndef DEBUG
#pragma inline_depth	( 255 )
#pragma inline_recursion( on )
#pragma intrinsic		(abs, fabs, fmod, sin, cos, tan, asin, acos, atan, sqrt, exp, log, log10, strcpy, strcat)
#define inline			__forceinline
#define _inline			__forceinline
#define __inline		__forceinline
#define IC				__forceinline
#endif

// Warnings
#pragma warning (disable : 4786 )		// too long names
#pragma warning (disable : 4503 )		// decorated name length exceeded
#pragma warning (disable : 4251 )		// object needs DLL interface

#define VERIFY(a) _ASSERTE(a)
#define R_ASSERT(a) assert(a)

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
using namespace std;

#ifdef _MIN 
#undef _MIN
#define _MIN(a,b) ((a<b)?a:b)
#endif

#ifdef _MAX 
#undef _MAX
#define _MAX(a,b) ((a>b)?a:b)
#endif

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E0B7368F_9D0A_4316_93C4_7E41943BC778__INCLUDED_)
