////////////////////////////////////////////////////////////////////////////
//	Module 		: script_space.h
//	Created 	: 22.09.2003
//  Modified 	: 01.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Script space
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

extern	char g_stdout[4096];

#pragma warning(pop)