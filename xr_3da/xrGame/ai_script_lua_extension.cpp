////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_lua_extension.cpp
//	Created 	: 19.09.2003
//  Modified 	: 19.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script extensions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_lua_extension.h"

using namespace Script;

int Script::ifSuspendThread (CLuaVirtualMachine *tpLuaVirtualMachine)
{
	return lua_yield(tpLuaVirtualMachine, lua_gettop(tpLuaVirtualMachine));
}

void Script::_Msg(LPCSTR caFormat,...)
{
	va_list	marker;

	va_start(marker,caFormat);

	Msg		(caFormat,marker);

	va_end	(marker);              
}

void Script::vfExportToLua(CLuaVirtualMachine *tpLuaVirtualMachine)
{
	open			(tpLuaVirtualMachine);
	lua_register	(tpLuaVirtualMachine,	"wait",								ifSuspendThread);
	function		(tpLuaVirtualMachine,	"printf",	(void (*) (LPCSTR))		(Log));
//	function		(tpLuaVirtualMachine,	"print",	(void (*) (LPCSTR,int))	(Log));
//	function		(tpLuaVirtualMachine,	"printf",	(void (*) (LPCSTR,...))	(Msg));
}