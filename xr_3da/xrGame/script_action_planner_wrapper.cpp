////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_planner_wrapper.cpp
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action planner wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_action_planner_wrapper.h"

void CScriptActionPlannerWrapper::reinit		(CLuaGameObject *object, bool clear_all)
{
	luabind::call_member<void>				(m_lua_instance,"reinit",object,clear_all);
}

void CScriptActionPlannerWrapper::reinit_static	(CScriptActionPlanner *planner, CLuaGameObject *object, bool clear_all)
{
	planner->CScriptActionPlanner::reinit	(object,clear_all);
}

void CScriptActionPlannerWrapper::update		(u32 time_delta)
{
	luabind::call_member<void>				(m_lua_instance,"update",time_delta);
}

void CScriptActionPlannerWrapper::update_static	(CScriptActionPlanner *planner, u32 time_delta)
{
	planner->CScriptActionPlanner::update	(time_delta);
}
