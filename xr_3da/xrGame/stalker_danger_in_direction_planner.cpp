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
	add_evaluator			(eWorldPropertyDanger				,xr_new<CStalkerPropertyEvaluatorDangers>			(m_object,"danger"));
	add_evaluator			(eWorldPropertyDangerUnknown		,xr_new<CStalkerPropertyEvaluatorConst>				(false,"fake"));
}

void CStalkerDangerInDirectionPlanner::add_actions					()
{
	CStalkerActionBase		*action;

	action					= xr_new<CStalkerActionDangerInDirectionTakeCover>(m_object,"fake");
	add_effect				(action,eWorldPropertyDanger,		false);
	add_operator			(eWorldOperatorDangerUnknownTakeCover,	action);
}
