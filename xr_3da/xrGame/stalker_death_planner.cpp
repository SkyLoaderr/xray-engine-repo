////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_death_planner.cpp
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker death planner
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_death_planner.h"
#include "stalker_death_actions.h"
#include "stalker_decision_space.h"
#include "stalker_property_evaluators.h"
#include "ai/stalker/ai_stalker.h"
#include "script_game_object.h"

using namespace StalkerDecisionSpace;

CStalkerDeathPlanner::CStalkerDeathPlanner	(CAI_Stalker *object, LPCSTR action_name) :
	inherited								(object,action_name)
{
}

CStalkerDeathPlanner::~CStalkerDeathPlanner	()
{
}

void CStalkerDeathPlanner::reinit			(CAI_Stalker *object, CPropertyStorage *storage, bool clear_all)
{
	inherited::reinit		(object,storage,clear_all);
	CScriptActionPlanner::m_storage.set_property	(eWorldPropertyDead,false);
}

void CStalkerDeathPlanner::reload			(LPCSTR section)
{
	inherited::reload		(section);
	clear					();
	add_evaluators			();
	add_actions				();
}

void CStalkerDeathPlanner::add_evaluators	()
{
	add_evaluator			(eWorldPropertyAlreadyDead		,xr_new<CStalkerPropertyEvaluatorConst>				(false));
	add_evaluator			(eWorldPropertyDead				,xr_new<CStalkerPropertyEvaluatorMember>			(CScriptActionBase::m_storage,eWorldPropertyDead,true));
}

void CStalkerDeathPlanner::add_actions		()
{
	CStalkerActionBase		*action;

	action					= xr_new<CStalkerActionDead>		(m_object,"dead");
	add_condition			(action,eWorldPropertyDead,			false);
	add_effect				(action,eWorldPropertyDead,			true);
	add_operator			(eWorldOperatorDead,				action);

	action					= xr_new<CStalkerActionBase>		(m_object,"already_dead");
	add_condition			(action,eWorldPropertyDead,			true);
	add_effect				(action,eWorldPropertyAlreadyDead,	true);
	add_operator			(eWorldOperatorAlreadyDead,			action);
}
