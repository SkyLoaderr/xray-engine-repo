////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.cpp
//	Created 	: 03.12.2004
//  Modified 	: 03.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Scripting engine
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_engine.h"
#include "script_space.h"

char g_stdout[4096];

extern void export_classes(lua_State *L);

CScriptEngine::CScriptEngine	()
{
	m_virtual_machine	= 0;
	m_virtual_machine	= lua_open();
	if (!m_virtual_machine) {
		ui().log		("ERROR : Cannot initialize script virtual machine!");
		return;
	}
	
	// initialize lua standard library functions 
	luaopen_base		(lua()); 
	luaopen_table		(lua());
	luaopen_string		(lua());
	luaopen_math		(lua());
#ifdef _DEBUG
	luaopen_debug		(lua());
#endif
	luabind::open		(lua());
}

CScriptEngine::~CScriptEngine	()
{
	if (m_virtual_machine)
		lua_close		(lua());
}

void CScriptEngine::init		()
{
	ui().log			("Scripting engine is initializing... ");
#ifdef _DEBUG
	ui().log			("\n");
#endif
	export_classes		(lua());
	ui().log			("completed\n");
}
