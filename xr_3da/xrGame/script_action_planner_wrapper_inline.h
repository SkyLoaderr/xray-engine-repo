////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_planner_wrapper_inline.h
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action planner wrapper inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CScriptActionPlannerWrapper::CScriptActionPlannerWrapper	(const luabind::object &lua_instance) :
	m_lua_instance		(lua_instance)
{
}

