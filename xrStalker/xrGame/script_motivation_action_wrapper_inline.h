////////////////////////////////////////////////////////////////////////////
//	Module 		: script_motivation_action_wrapper_inline.h
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script motivation action wrapper inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CScriptMotivationActionWrapper::CScriptMotivationActionWrapper	(const luabind::object &lua_instance, const CGraphEngine::CWorldState &goal) :
	inherited						(goal),
	m_lua_instance					(lua_instance)
{
}
