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
#include <math.h>
#include <float.h>

#define IC					__forceinline

using namespace std;

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

// SCRIPTS  only
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "src/xr_LuaAllocator.h"

#define LUA_API

#define xr_strlen strlen
#define xr_strcpy strcpy
#define xr_strcmp strcmp

// generic
template <class T>	IC T		_min	(T a, T b)	{ return a<b?a:b;	}
template <class T>	IC T		_max	(T a, T b)	{ return a>b?a:b;	}
template <class T>	IC T		_sqr	(T a)		{ return a*a;		}

// float
IC float	_abs	(float x)		{ return fabsf(x); }
IC float	_sqrt	(float x)		{ return sqrtf(x); }
IC float	_sin	(float x)		{ return sinf(x); }
IC float	_cos	(float x)		{ return cosf(x); }
IC BOOL		_valid	(const float x)
{
	// check for: Signaling NaN, Quiet NaN, Negative infinity ( ÖINF), Positive infinity (+INF), Negative denormalized, Positive denormalized
	int			cls			= _fpclass		(double(x));
	if (cls&(_FPCLASS_SNAN+_FPCLASS_QNAN+_FPCLASS_NINF+_FPCLASS_PINF+_FPCLASS_ND+_FPCLASS_PD))	
		return	false;	

	/*	*****other cases are*****
	_FPCLASS_NN Negative normalized non-zero 
	_FPCLASS_NZ Negative zero ( Ö 0) 
	_FPCLASS_PZ Positive 0 (+0) 
	_FPCLASS_PN Positive normalized non-zero 
	*/
	return		true;
}


// double
IC double	_abs	(double x)		{ return fabs(x); }
IC double	_sqrt	(double x)		{ return sqrt(x); }
IC double	_sin	(double x)		{ return sin(x); }
IC double	_cos	(double x)		{ return cos(x); }
IC BOOL		_valid	(const double x)
{
	// check for: Signaling NaN, Quiet NaN, Negative infinity ( ÖINF), Positive infinity (+INF), Negative denormalized, Positive denormalized
	int			cls			= _fpclass		(x);
	if (cls&(_FPCLASS_SNAN+_FPCLASS_QNAN+_FPCLASS_NINF+_FPCLASS_PINF+_FPCLASS_ND+_FPCLASS_PD))	
		return false;	

	/*	*****other cases are*****
	_FPCLASS_NN Negative normalized non-zero 
	_FPCLASS_NZ Negative zero ( Ö 0) 
	_FPCLASS_PZ Positive 0 (+0) 
	_FPCLASS_PN Positive normalized non-zero 
	*/
	return		true;
}

// int8
IC s8		_abs	(s8  x)			{ return (x>=0)? x : s8(-x); }
IC s8 		_min	(s8  x, s8  y)	{ return y + ((x - y) & ((x - y) >> (sizeof(s8 ) * 8 - 1))); };
IC s8 		_max	(s8  x, s8  y)	{ return x - ((x - y) & ((x - y) >> (sizeof(s8 ) * 8 - 1))); };

// unsigned int8
IC u8		_abs	(u8 x)			{ return x; }

// int16
IC s16		_abs	(s16 x)			{ return (x>=0)? x : s16(-x); }
IC s16		_min	(s16 x, s16 y)	{ return y + ((x - y) & ((x - y) >> (sizeof(s16) * 8 - 1))); };
IC s16		_max	(s16 x, s16 y)	{ return x - ((x - y) & ((x - y) >> (sizeof(s16) * 8 - 1))); };

// unsigned int16
IC u16		_abs	(u16 x)			{ return x; }

// int32
IC s32		_abs	(s32 x)			{ return (x>=0)? x : s32(-x); }
IC s32		_min	(s32 x, s32 y)	{ return y + ((x - y) & ((x - y) >> (sizeof(s32) * 8 - 1))); };
IC s32		_max	(s32 x, s32 y)	{ return x - ((x - y) & ((x - y) >> (sizeof(s32) * 8 - 1))); };

// int64
IC s64		_abs	(s64 x)			{ return (x>=0)? x : s64(-x); }
IC s64		_min	(s64 x, s64 y)	{ return y + ((x - y) & ((x - y) >> (sizeof(s64) * 8 - 1))); };
IC s64		_max	(s64 x, s64 y)	{ return x - ((x - y) & ((x - y) >> (sizeof(s64) * 8 - 1))); };

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#ifndef _DEBUG
#	define LUABIND_NO_ERROR_CHECKING
#endif

#define LUABIND_NO_EXCEPTIONS
#define LUABIND_DONT_COPY_STRINGS
#define BOOST_THROW_EXCEPTION_HPP_INCLUDED

namespace boost {	void __stdcall throw_exception(const std::exception &A);	};

// end of SCRIPTS ONLY