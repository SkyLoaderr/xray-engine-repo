// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
// Third generation by Oles.

#ifndef stdafxH
#define stdafxH

#pragma once

#include <xrCore.h>

//#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "src\\xr_LuaAllocator.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#ifdef __BORLANDC__
#pragma comment( lib, "x:\\xrCoreB.lib"	)
#else
#pragma comment( lib, "x:\\xrCore.lib"	)
#endif

#ifndef DEBUG
#	define LUABIND_NO_ERROR_CHECKING
#endif

#if !defined(DEBUG) || defined(FORCE_NO_EXCEPTIONS)
	// release: no error checking, no exceptions
	#define LUABIND_NO_EXCEPTIONS
	#define BOOST_THROW_EXCEPTION_HPP_INCLUDED
	namespace boost {	void __stdcall throw_exception(const std::exception &A);	};
#endif
#define LUABIND_DONT_COPY_STRINGS

#endif //stdafxH
