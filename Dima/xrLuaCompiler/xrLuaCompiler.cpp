////////////////////////////////////////////////////////////////////////////
//	Module 		: xrLuaCompiler.cpp
//	Created 	: 14.10.2003
//  Modified 	: 14.10.2003
//	Author		: Dmitriy Iassenev
//	Description : X-Ray Lua Compiler
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "xrCore.h"
#pragma comment(lib,"x:\\xrCore.lib")

// Lua
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#pragma comment(lib,"x:\\xrLUA.lib")

// Lua-bind
#pragma warning(disable:4244)
#include "luabind\\luabind.hpp""
#include "luabind\\adopt_policy.hpp"
#include "luabind\\dependency_policy.hpp"
#include "luabind\\return_reference_to_policy.hpp"
#include "luabind\\out_value_policy.hpp"
#include "luabind\\discard_result_policy.hpp"
#include "luabind\\iterator_policy.hpp"
#pragma warning(default:4244)

// I need this because we have to exclude option /EHsc (exception handling) from the project
namespace boost {
	void __stdcall throw_exception(const exception &A)
	{
		Debug.fatal("Boost exception raised %s",A.what());
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}