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

void CScriptActionPlannerWrapper::setup			(CScriptGameObject *object)
{
	luabind::call_member<void>				(this,"setup",object);
}

void CScriptActionPlannerWrapper::setup_static	(CScriptActionPlanner *planner, CScriptGameObject *object)
{
	planner->CScriptActionPlanner::setup	(object);
}

void CScriptActionPlannerWrapper::update		()
{
	luabind::call_member<void>				(this,"update");
}

void CScriptActionPlannerWrapper::update_static	(CScriptActionPlanner *planner)
{
	planner->CScriptActionPlanner::update	();
}
