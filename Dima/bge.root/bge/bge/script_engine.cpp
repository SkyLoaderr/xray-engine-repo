////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.cpp
//	Created 	: 03.12.2004
//  Modified 	: 03.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Scripting engine
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_engine.h"
#include "ui.h"
#include "script_space.h"

CScriptEngine::CScriptEngine	()
{
	m_virtual_machine		= 0;
	m_virtual_machine		= lua_open();
	if (!m_virtual_machine) {
		ui().log			("ERROR : Cannot initialize script virtual machine!");
		return;
	}
	
	// initialize lua standard library functions 
	luaopen_base			(lua()); 
	luaopen_table			(lua());
	luaopen_string			(lua());
	luaopen_math			(lua());
#ifdef DEBUG
	luaopen_debug			(lua());
#endif
}

CScriptEngine::~CScriptEngine	()
{
}

void CScriptEngine::init		()
{
	ui().log	("Scripting engine is initialized\n");
}
