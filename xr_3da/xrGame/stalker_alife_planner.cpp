////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_alife_planner.cpp
//	Created 	: 25.03.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker ALife planner
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_alife_planner.h"
#include "stalker_alife_actions.h"
#include "stalker_decision_space.h"
#include "stalker_property_evaluators.h"
#include "ai/stalker/ai_stalker.h"
#include "script_game_object.h"
#include "../skeletonanimated.h"

using namespace StalkerDecisionSpace;

#ifdef _DEBUG
//#	define STALKER_DEBUG_MODE
#endif

#ifdef STALKER_DEBUG_MODE
#	include "attachable_item.h"
#endif

CStalkerALifePlanner::CStalkerALifePlanner	(CAI_Stalker *object, LPCSTR action_name) :
	inherited								(object,action_name)
{
}

CStalkerALifePlanner::~CStalkerALifePlanner	()
{
}

void CStalkerALifePlanner::reinit			(CAI_Stalker *object, CPropertyStorage *storage, bool clear_all)
{
	inherited::reinit		(object,storage,clear_all);
}

void CStalkerALifePlanner::reload			(LPCSTR section)
{
	inherited::reload		(section);
	clear					();
	add_evaluators			();
	add_actions				();
}

void CStalkerALifePlanner::add_evaluators	()
{
	add_evaluator			(eWorldPropertyPuzzleSolved			,xr_new<CStalkerPropertyEvaluatorConst>					(false));
	add_evaluator			(eWorldPropertyItems				,xr_new<CStalkerPropertyEvaluatorItems>					());
	add_evaluator			(eWorldPropertyALife				,xr_new<CStalkerPropertyEvaluatorALife>					());
	add_evaluator			(eWorldPropertyReachedTaskLocation	,xr_new<CStalkerPropertyEvaluatorReachedTaskLocation>	());
	add_evaluator			(eWorldPropertyTaskCompleted		,xr_new<CStalkerPropertyEvaluatorTaskAccomplished>		());
	add_evaluator			(eWorldPropertyCustomerSatisfied	,xr_new<CStalkerPropertyEvaluatorCustomerSatisfied>		());
}

void CStalkerALifePlanner::add_actions		()
{
	CStalkerActionBase		*action;

	action					= xr_new<CStalkerActionGatherItems>	(m_object,"gather_items");
	add_condition			(action,eWorldPropertyItems,		true);
	add_effect				(action,eWorldPropertyItems,		false);
	add_operator			(eWorldOperatorGatherItems,			action);

	action					= xr_new<CStalkerActionFreeNoALife>	(m_object,"free_no_alife");
	add_condition			(action,eWorldPropertyALife,		false);
	add_condition			(action,eWorldPropertyItems,		false);
	add_condition			(action,eWorldPropertyPuzzleSolved,	false);
	add_effect				(action,eWorldPropertyPuzzleSolved,	true);
	add_operator			(eWorldOperatorFreeNoALife,			action);

	action					= xr_new<CStalkerActionFreeALife>	(m_object,"free_alife");
	add_condition			(action,eWorldPropertyALife,		true);
	add_condition			(action,eWorldPropertyItems,		false);
	add_condition			(action,eWorldPropertyPuzzleSolved,	false);
	add_condition			(action,eWorldPropertyTaskCompleted,true);
	add_effect				(action,eWorldPropertyPuzzleSolved,	true);
	add_operator			(eWorldOperatorFreeALife,			action);

	action					= xr_new<CStalkerActionReachTaskLocation>	(m_object,"reach_task_location");
	add_condition			(action,eWorldPropertyALife,		true);
	add_condition			(action,eWorldPropertyItems,		false);
	add_condition			(action,eWorldPropertyReachedTaskLocation,	false);
	add_effect				(action,eWorldPropertyReachedTaskLocation,	true);
	add_operator			(eWorldOperatorReachTaskLocation,	action);

	action					= xr_new<CStalkerActionAccomplishTask>	(m_object,"accomplish_task");
	add_condition			(action,eWorldPropertyALife,		true);
	add_condition			(action,eWorldPropertyItems,		false);
	add_condition			(action,eWorldPropertyReachedTaskLocation,	true);
	add_condition			(action,eWorldPropertyTaskCompleted,false);
	add_effect				(action,eWorldPropertyTaskCompleted,true);
	add_operator			(eWorldOperatorAccomplishTask,		action);

	action					= xr_new<CStalkerActionFollowCustomer>	(m_object,"go_to_customer");
	add_condition			(action,eWorldPropertyALife,		true);
	add_condition			(action,eWorldPropertyItems,		false);
	add_condition			(action,eWorldPropertyReachedTaskLocation,	true);
	add_condition			(action,eWorldPropertyTaskCompleted,true);
	add_condition			(action,eWorldPropertyCustomerSatisfied,false);
	add_effect				(action,eWorldPropertyCustomerSatisfied,true);
	add_operator			(eWorldOperatorFollowCustomer,		action);
}
