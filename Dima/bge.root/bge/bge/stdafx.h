////////////////////////////////////////////////////////////////////////////
//	Module 		: stdafx.h
//	Created 	: 01.10.2004
//  Modified 	: 01.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Standard precompiled header
////////////////////////////////////////////////////////////////////////////

#pragma once

#define _CRT_INSECURE_DEPRECATE

#include <algorithm>
#include <vector>
#include <list>
#include <stack>
#include <map>
#include <windows.h>

// types
typedef	signed		char	s8;
typedef	unsigned	char	u8;

typedef	signed		short	s16;
typedef	unsigned	short	u16;

typedef	signed		int		s32;
typedef	unsigned	int		u32;

typedef	signed		__int64	s64;
typedef	unsigned	__int64	u64;

typedef float				f32;
typedef double				f64;

typedef char*				LPSTR;
typedef const char*			LPCSTR;

#define IC					__forceinline

#ifndef _DEBUG
	#pragma inline_depth	( 254 )
	#pragma inline_recursion( on )
	#pragma intrinsic		(abs, fabs, fmod, sin, cos, tan, asin, acos, atan, sqrt, exp, log, log10, strcpy, strcat)
#endif

#ifdef _DEBUG
	#define	NODEFAULT		{\
		ui().log("nodefault reached : %s(%s)",__FILE__, __LINE__);\
		__asm int 3\
	}

	#define VERIFY(expr)	{\
		if (!(expr)) {\
			ui().log("%s : %s(%s)",#expr,__FILE__, __LINE__);\
			__asm int 3\
		}\
	}
#else
	#ifdef __BORLANDC__
		#define NODEFAULT
    #else
		#define NODEFAULT	__assume(0)
    #endif

	#define VERIFY(expr)
#endif

template<typename T>
IC	void c_delete	(T *&ptr)
{
	if (ptr) {
		delete		ptr;
		ptr			= 0;
	}
}

template<typename T>
IC	void c_free		(T *&ptr)
{
	if (ptr) {
		free		(ptr);
		ptr			= 0;
	}
}
