#ifndef xrDebug_macrosH
#define xrDebug_macrosH
#pragma once

#ifdef __BORLANDC__
#	define __FUNCTION__ "cannot invoke function"
#endif // __BORLANDC__

// ---==( Extended Debugging Support (R) )==---
#define DEBUG_INFO					__FILE__,__LINE__,__FUNCTION__
#define R_ASSERT(expr)				do {if (!(expr)) ::Debug.fail(#expr,DEBUG_INFO);} while(0)
#define R_ASSERT2(expr,e2)			do {if (!(expr)) ::Debug.fail(#expr,e2,DEBUG_INFO);} while(0)
#define R_ASSERT3(expr,e2,e3)		do {if (!(expr)) ::Debug.fail(#expr,e2,e3,DEBUG_INFO);} while(0)
#define R_CHK(expr)					do {HRESULT hr = expr; if (FAILED(hr)) ::Debug.error(hr,#expr,DEBUG_INFO);} while(0)

#ifdef VERIFY
#	undef VERIFY
#endif // VERIFY

#ifdef DEBUG
#	define FATAL(description)		Debug.fatal(DEBUG_INFO,description)
#	define NODEFAULT				FATAL("nodefault reached")
#	define VERIFY(expr)				do {if (!(expr)) ::Debug.fail(#expr,DEBUG_INFO);} while(0)
#	define VERIFY2(expr, e2)		do {if (!(expr)) ::Debug.fail(#expr,e2,DEBUG_INFO);} while(0)
#	define VERIFY3(expr, e2, e3)	do {if (!(expr)) ::Debug.fail(#expr,e2,e3,DEBUG_INFO);} while(0)
#	define CHK_DX(expr)				do {HRESULT hr = expr; if (FAILED(hr)) ::Debug.error(hr,#expr,DEBUG_INFO);} while(0)
#else
#	ifdef __BORLANDC__
#		define NODEFAULT
#	else
#		define NODEFAULT __assume(0)
#	endif
#	define VERIFY(expr)
#	define VERIFY2(expr, e2)
#	define VERIFY3(expr, e2, e3)
#	define CHK_DX(a) a
#endif

//---------------------------------------------------------------------------------------------
// FIXMEs / TODOs / NOTE macros
//---------------------------------------------------------------------------------------------
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define NOTE( x )  message( x )
#define FILE_LINE  message( __FILE__LINE__ )

#define TODO( x )  message( __FILE__LINE__"\n"           \
	" ------------------------------------------------\n" \
	"|  TODO :   " #x "\n" \
	" -------------------------------------------------\n" )
#define FIXME( x )  message(  __FILE__LINE__"\n"           \
	" ------------------------------------------------\n" \
	"|  FIXME :  " #x "\n" \
	" -------------------------------------------------\n" )
#define todo( x )  message( __FILE__LINE__" TODO :   " #x "\n" ) 
#define fixme( x )  message( __FILE__LINE__" FIXME:   " #x "\n" ) 

//--------- static assertion
template<bool>	struct CompileTimeError;
template<>		struct CompileTimeError<true>	{};
#define STATIC_CHECK(expr, msg) \
{ \
	CompileTimeError<((expr) != 0)> ERROR_##msg; \
	(void)ERROR_##msg; \
}
#endif