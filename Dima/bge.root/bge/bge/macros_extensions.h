////////////////////////////////////////////////////////////////////////////
//	Module 		: macros_extensions.h
//	Created 	: 03.12.2004
//  Modified 	: 03.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Macros extensions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define IC					__forceinline

#ifndef _DEBUG
	#pragma inline_depth	( 254 )
	#pragma inline_recursion( on )
	#pragma intrinsic		(abs, fabs, fmod, sin, cos, tan, asin, acos, atan, sqrt, exp, log, log10, strcpy, strcat)
#endif

#ifdef _DEBUG
	#define	NODEFAULT		{\
		ui().log("nodefault reached : %s(%s)",__FILE__, __LINE__);\
		ui().flush();\
		__asm int 3\
	}

	#define VERIFY(expr)	{\
		if (!(expr)) {\
			ui().log("\n\nAssertion failed!\nexpression : \"%s\"\nfile       : %s\nline       : %d\n",#expr,__FILE__, __LINE__);\
			ui().flush();\
			__asm int 3\
		}\
	}

	#define VERIFY2(expr,str)	{\
		if (!(expr)) {\
			ui().log("\n\nAssertion failed!\nexpression : \"%s\"\ndescription : \"%s\"\nfile       : %s\nline       : %d\n",#expr,str,__FILE__, __LINE__);\
			ui().flush();\
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
	#define VERIFY2(expr)
#endif
