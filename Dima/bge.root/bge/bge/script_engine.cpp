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

string4096 g_stdout;

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
	ui().log			("Script engine is initializing...");
#ifdef _DEBUG
	ui().log			("\n");
#endif
	export_classes		(lua());

	lua_dostring		(lua(),"function printf(fmt,...) ui():log(string.format(fmt,unpack(arg))) end");
	
	ui().log			("completed\n\n");
}

void CScriptEngine::run_string	(LPCSTR string_to_run)
{
	lua_dostring		(lua(),string_to_run);
#ifdef _DEBUG
	lua_setgcthreshold	(lua(),0);
#endif
}
