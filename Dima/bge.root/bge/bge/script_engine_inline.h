////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine_inline.h
//	Created 	: 03.12.2004
//  Modified 	: 03.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Scripting engine inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	lua_State *CScriptEngine::lua	() const
{
	VERIFY	(m_virtual_machine);
	return	(m_virtual_machine);
}
