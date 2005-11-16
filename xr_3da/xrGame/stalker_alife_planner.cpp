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
#include "stalker_alife_task_actions.h"
#include "stalker_decision_space.h"
#include "stalker_property_evaluators.h"
#include "ai/stalker/ai_stalker.h"
#include "script_game_object.h"

using namespace StalkerDecisionSpace;

CStalkerALifePlanner::CStalkerALifePlanner	(CAI_Stalker *object, LPCSTR action_name) :
	inherited								(object,action_name)
{
}

CStalkerALifePlanner::~CStalkerALifePlanner	()
{
}

void CStalkerALifePlanner::setup			(CAI_Stalker *object, CPropertyStorage *storage)
{
	inherited::setup		(object,storage);

	CScriptActionPlanner::m_storage.set_property(eWorldPropertyReachedTaskLocation,false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyTaskCompleted,false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyReachedCustomerLocation,false);
	CScriptActionPlanner::m_storage.set_property(eWorldPropertyCustomerSatisfied,false);
	
	clear					();
	add_evaluators			();
	add_actions				();
}

void CStalkerALifePlanner::add_evaluators	()
{
	add_evaluator			(eWorldPropertyPuzzleSolved				,xr_new<CStalkerPropertyEvaluatorConst>						(false,"zone puzzle solved"));
	add_evaluator			(eWorldPropertyCustomerSatisfied		,xr_new<CStalkerPropertyEvaluatorConst>						(false,"customer satisfied"));

	add_evaluator			(eWorldPropertyReachedTaskLocation		,xr_new<CStalkerPropertyEvaluatorMember>					(CScriptActionBase::m_storage,eWorldPropertyReachedTaskLocation,true,true,"reached task location"));
	add_evaluator			(eWorldPropertyTaskCompleted			,xr_new<CStalkerPropertyEvaluatorMember>					(CScriptActionBase::m_storage,eWorldPropertyTaskCompleted,true,true,"task completed"));
	add_evaluator			(eWorldPropertyReachedCustomerLocation	,xr_new<CStalkerPropertyEvaluatorMember>					(CScriptActionBase::m_storage,eWorldPropertyReachedCustomerLocation,true,true,"reached customer location"));
	
	add_evaluator			(eWorldPropertySmartTerrainTask			,xr_new<CStalkerPropertyEvaluatorSmartTerrainTask>			(m_object,"under smart terrain"));
	add_evaluator			(eWorldPropertyALife					,xr_new<CStalkerPropertyEvaluatorALife>						(m_object,"ALife Simulator"));
}

void CStalkerALifePlanner::add_actions		()
{
	CStalkerActionBase		*action;

	action					= xr_new<CStalkerActionNoALife>					(m_object,"free_no_alife");
	add_condition			(action,eWorldPropertyALife,					false);
	add_condition			(action,eWorldPropertyPuzzleSolved,				false);
	add_effect				(action,eWorldPropertyPuzzleSolved,				true);
	add_operator			(eWorldOperatorALifeEmulation,					action);

	action					= xr_new<CStalkerActionSmartTerrain>			(m_object,"smart terrain : get task location");
	add_condition			(action,eWorldPropertyALife,					true);
	add_condition			(action,eWorldPropertySmartTerrainTask,			true);
	add_effect				(action,eWorldPropertySmartTerrainTask,			false);
	add_operator			(eWorldOperatorSmartTerrainTask,				action);

	action					= xr_new<CStalkerActionSolveZonePuzzle>			(m_object,"solve_zone_puzzle");
	add_condition			(action,eWorldPropertyALife,					true);
	add_condition			(action,eWorldPropertySmartTerrainTask,			false);
	add_condition			(action,eWorldPropertyPuzzleSolved,				false);
	add_condition			(action,eWorldPropertyCustomerSatisfied,		true);
	add_effect				(action,eWorldPropertyPuzzleSolved,				true);
	add_operator			(eWorldOperatorSolveZonePuzzle,					action);

	action					= xr_new<CStalkerActionReachTaskLocation>		(m_object,"reach_task_location");
	add_condition			(action,eWorldPropertyALife,					true);
	add_condition			(action,eWorldPropertySmartTerrainTask,			false);
	add_condition			(action,eWorldPropertyReachedTaskLocation,		false);
	add_effect				(action,eWorldPropertyReachedTaskLocation,		true);
	add_operator			(eWorldOperatorReachTaskLocation,				action);

	action					= xr_new<CStalkerActionAccomplishTask>			(m_object,"accomplish_task");
	add_condition			(action,eWorldPropertyALife,					true);
	add_condition			(action,eWorldPropertySmartTerrainTask,			false);
	add_condition			(action,eWorldPropertyReachedTaskLocation,		true);
	add_condition			(action,eWorldPropertyTaskCompleted,			false);
	add_effect				(action,eWorldPropertyTaskCompleted,			true);
	add_operator			(eWorldOperatorAccomplishTask,					action);

	action					= xr_new<CStalkerActionReachCustomerLocation>	(m_object,"go_to_customer : task completed");
	add_condition			(action,eWorldPropertyALife,					true);
	add_condition			(action,eWorldPropertySmartTerrainTask,			false);
	add_condition			(action,eWorldPropertyReachedTaskLocation,		true);
	add_condition			(action,eWorldPropertyTaskCompleted,			true);
	add_condition			(action,eWorldPropertyReachedCustomerLocation,	false);
	add_effect				(action,eWorldPropertyReachedCustomerLocation,	true);
	add_operator			(eWorldOperatorReachCustomerLocation,			action);
	
	action					= xr_new<CStalkerActionCommunicateWithCustomer>	(m_object,"statisfy_customer");
	add_condition			(action,eWorldPropertyALife,					true);
	add_condition			(action,eWorldPropertySmartTerrainTask,			false);
	add_condition			(action,eWorldPropertyReachedCustomerLocation,	true);
	add_condition			(action,eWorldPropertyCustomerSatisfied,		false);
	add_effect				(action,eWorldPropertyCustomerSatisfied,		true);
	add_operator			(eWorldOperatorCommunicateWithCustomer,			action);
}
