////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_wrapper_inline.h
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action wrapper inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CScriptActionWrapper::CScriptActionWrapper	(const luabind::object &lua_instance, CLuaGameObject *object, LPCSTR action_name) : 
	CScriptAction					(object,action_name),
	m_lua_instance					(lua_instance)
{
}

