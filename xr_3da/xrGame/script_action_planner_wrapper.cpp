////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_planner_wrapper.cpp
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action planner wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_action_planner_wrapper.h"
#include "ai_script_classes.h"

void CScriptActionPlannerWrapper::reinit		(CLuaGameObject *object, bool clear_all)
{
	call_member<void>				("reinit",object,clear_all);
}

void CScriptActionPlannerWrapper::reinit_static	(CScriptActionPlanner *planner, CLuaGameObject *object, bool clear_all)
{
	planner->CScriptActionPlanner::reinit	(object,clear_all);
}

void CScriptActionPlannerWrapper::update		(u32 time_delta)
{
	call_member<void>				("update",time_delta);
}

void CScriptActionPlannerWrapper::update_static	(CScriptActionPlanner *planner, u32 time_delta)
{
	planner->CScriptActionPlanner::update	(time_delta);
}
