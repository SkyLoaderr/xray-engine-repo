////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_in_direction_planner.cpp
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger in direction planner class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_danger_in_direction_planner.h"
#include "ai/stalker/ai_stalker.h"
#include "script_game_object.h"
#include "stalker_danger_in_direction_actions.h"
#include "stalker_decision_space.h"
#include "stalker_danger_property_evaluators.h"
#include "agent_manager.h"
#include "agent_member_manager.h"

using namespace StalkerDecisionSpace;

CStalkerDangerInDirectionPlanner::CStalkerDangerInDirectionPlanner	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerDangerInDirectionPlanner::setup						(CAI_Stalker *object, CPropertyStorage *storage)
{
	inherited::setup		(object,storage);
	clear					();
	add_evaluators			();
	add_actions				();
}

void CStalkerDangerInDirectionPlanner::initialize					()
{
	inherited::initialize	();
	
	object().agent_manager().member().member(&object()).cover(0);

	CScriptActionPlanner::m_storage.set_property(eWorldPropertyCoverReached,false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyLookedAround,false);
}

void CStalkerDangerInDirectionPlanner::update						()
{
	inherited::update		();
}

void CStalkerDangerInDirectionPlanner::finalize						()
{
	inherited::finalize		();
}

void CStalkerDangerInDirectionPlanner::add_evaluators				()
{
	add_evaluator			(eWorldPropertyDanger			,xr_new<CStalkerPropertyEvaluatorDangers>					(m_object,"danger"));
	add_evaluator			(eWorldPropertyCoverActual		,xr_new<CStalkerPropertyEvaluatorDangerUnknownCoverActual>	(m_object,"danger in direction : cover actual"));
	add_evaluator			(eWorldPropertyCoverReached		,xr_new<CStalkerPropertyEvaluatorMember>					(CScriptActionBase::m_storage,eWorldPropertyCoverReached,true,true,"danger in direction : cover reached"));
	add_evaluator			(eWorldPropertyLookedAround		,xr_new<CStalkerPropertyEvaluatorMember>					(CScriptActionBase::m_storage,eWorldPropertyLookedAround,true,true,"danger in direction : looked around"));
}

void CStalkerDangerInDirectionPlanner::add_actions					()
{
	CStalkerActionBase		*action;

	action					= xr_new<CStalkerActionDangerInDirectionTakeCover>	(m_object,"take cover");
	add_effect				(action,eWorldPropertyCoverActual,		true);
	add_effect				(action,eWorldPropertyCoverReached,		true);
	add_operator			(eWorldOperatorDangerInDirectionTakeCover,	action);

	action					= xr_new<CStalkerActionDangerInDirectionLookAround>	(m_object,"look around");
	add_condition			(action,eWorldPropertyCoverActual,		true);
	add_condition			(action,eWorldPropertyCoverReached,		true);
	add_condition			(action,eWorldPropertyLookedAround,		false);
	add_effect				(action,eWorldPropertyLookedAround,		true);
	add_operator			(eWorldOperatorDangerInDirectionLookAround,	action);

	action					= xr_new<CStalkerActionDangerInDirectionSearch>		(m_object,"search");
	add_condition			(action,eWorldPropertyCoverActual,		true);
	add_condition			(action,eWorldPropertyCoverReached,		true);
	add_condition			(action,eWorldPropertyLookedAround,		true);
	add_effect				(action,eWorldPropertyDanger,			false);
	add_operator			(eWorldOperatorDangerInDirectionSearchEnemy,action);
}
