#pragma once
#include <windows.h>
#include <mmsystem.h>

#ifndef _DEBUG
#pragma inline_depth	( 254 )
#pragma inline_recursion( on )
//#pragma intrinsic		(abs, fabs, fmod, sin, cos, tan, asin, acos, atan, sqrt, exp, log, log10, strcpy, strcat)
#define inline			__forceinline
#define _inline			__forceinline
#define __inline		__forceinline
#define IC				__forceinline
#else
#define IC				__forceinline
#endif

#include <algorithm>

