////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_planner_action_wrapper.cpp
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action planner action wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_action_planner_action_wrapper.h"
#include "script_game_object.h"

void CScriptActionPlannerActionWrapper::reinit		(CScriptGameObject *object, CPropertyStorage *storage, bool clear_all)
{
	call_member<void>				("reinit",object,storage,clear_all);
}

void CScriptActionPlannerActionWrapper::reinit_static	(CScriptActionPlannerAction *planner, CScriptGameObject *object, CPropertyStorage *storage, bool clear_all)
{
	planner->CScriptActionPlannerAction::reinit	(object,storage,clear_all);
}

void CScriptActionPlannerActionWrapper::update		(u32 time_delta)
{
	call_member<void>				("update",time_delta);
}

void CScriptActionPlannerActionWrapper::update_static	(CScriptActionPlannerAction *planner, u32 time_delta)
{
	planner->CScriptActionPlannerAction::update	(time_delta);
}

void CScriptActionPlannerActionWrapper::initialize			()
{
	call_member<void>			("initialize");
}

void CScriptActionPlannerActionWrapper::initialize_static	(CScriptActionPlannerAction *action)
{
	action->CScriptActionPlannerAction::initialize	();
}

void CScriptActionPlannerActionWrapper::execute				()
{
	call_member<void>			("execute");
}

void CScriptActionPlannerActionWrapper::execute_static		(CScriptActionPlannerAction *action)
{
	action->CScriptActionPlannerAction::execute		();
}

void CScriptActionPlannerActionWrapper::finalize				()
{
	call_member<void>			("finalize");
}

void CScriptActionPlannerActionWrapper::finalize_static		(CScriptActionPlannerAction *action)
{
	action->CScriptActionPlannerAction::finalize		();
}

CScriptActionPlannerActionWrapper::_edge_value_type CScriptActionPlannerActionWrapper::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
	return								(const_cast<CScriptActionPlannerActionWrapper*>(this)->call_member<_edge_value_type>("weight",condition0,condition1));
}

CScriptActionPlannerActionWrapper::_edge_value_type CScriptActionPlannerActionWrapper::weight_static	(CScriptActionPlannerAction *action, const CSConditionState &condition0, const CSConditionState &condition1)
{
	return								(((const CScriptActionPlannerActionWrapper*)action)->CScriptActionPlannerAction::weight(condition0,condition1));
}
