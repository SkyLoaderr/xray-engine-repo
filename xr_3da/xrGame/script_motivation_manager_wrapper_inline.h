////////////////////////////////////////////////////////////////////////////
//	Module 		: script_motivation_action_manager_wrapper_inline.h
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script motivation action manager wrapper inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CScriptMotivationManagerWrapper::CScriptMotivationManagerWrapper	(luabind::weak_ref lua_instance) :
	m_lua_instance					(lua_instance)
{
}
