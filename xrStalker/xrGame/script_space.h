////////////////////////////////////////////////////////////////////////////
//	Module 		: script_space.h
//	Created 	: 22.09.2003
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script space
////////////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning(disable:4244)
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#pragma warning(disable:4267)

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <luabind/luabind.hpp>
#include <luabind/object.hpp>

__declspec(dllimport) LPSTR g_ca_stdout;

#pragma warning(default:4244)
#pragma warning(default:4995)
#pragma warning(default:4530)
#pragma warning(default:4267)

typedef lua_State CLuaVirtualMachine;

struct SMemberCallback {
	luabind::functor<void>	*m_lua_function;
	luabind::object			*m_lua_object;
	ref_str					m_method_name;
};
