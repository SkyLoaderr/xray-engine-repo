////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_planner_wrapper.cpp
//	Created 	: 19.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action planner wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_action_planner_wrapper.h"
#include "script_game_object.h"

void CScriptActionPlannerWrapper::reinit		(CScriptGameObject *object, bool clear_all)
{
	luabind::call_member<void>				(this,"reinit",object,clear_all);
}

void CScriptActionPlannerWrapper::reinit_static	(CScriptActionPlanner *planner, CScriptGameObject *object, bool clear_all)
{
	planner->CScriptActionPlanner::reinit	(object,clear_all);
}

void CScriptActionPlannerWrapper::update		()
{
	luabind::call_member<void>				(this,"update");
}

void CScriptActionPlannerWrapper::update_static	(CScriptActionPlanner *planner)
{
	planner->CScriptActionPlanner::update	();
}
