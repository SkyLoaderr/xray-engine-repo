////////////////////////////////////////////////////////////////////////////
//	Module 		: script_motivation_wrapper_inline.h
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script motivation wrapper inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CScriptMotivationWrapper::CScriptMotivationWrapper	(const luabind::object &lua_instance) : 
	m_lua_instance					(lua_instance)
{
}

