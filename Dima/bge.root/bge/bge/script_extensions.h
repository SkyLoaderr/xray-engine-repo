////////////////////////////////////////////////////////////////////////////
//	Module 		: script_extensions.h
//	Created 	: 03.12.2004
//  Modified 	: 03.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Script extensions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "memory.h"
#include "src/xr_LuaAllocator.h"

#define xr_strlen strlen
#define xr_strcpy strcpy
#define xr_strcmp strcmp

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#ifndef _DEBUG
#	define LUABIND_NO_ERROR_CHECKING
#endif

#define LUABIND_NO_EXCEPTIONS
#define LUABIND_DONT_COPY_STRINGS
#define BOOST_THROW_EXCEPTION_HPP_INCLUDED

namespace boost {	void throw_exception(const std::exception &A);	};
