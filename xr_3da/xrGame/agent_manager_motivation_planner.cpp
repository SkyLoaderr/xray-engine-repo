////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_motivation_planner.cpp
//	Created 	: 24.05.2004
//  Modified 	: 02.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager motivation planner
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "agent_manager.h"
#include "agent_manager_space.h"
#include "agent_manager_actions.h"
#include "agent_manager_motivations.h"
#include "agent_manager_properties.h"
#include "agent_manager_motivation_planner.h"

using namespace AgentManager;

void CAgentManagerMotivationPlanner::setup	(CAgentManager *object)
{
	inherited::setup	(object);
	clear				();
	add_motivations		();
	add_evaluators		();
	add_actions			();
}

void CAgentManagerMotivationPlanner::add_motivations	()
{
	CWorldState				goal;

	goal.clear				();
	goal.add_condition		(CWorldProperty(ePropertyOrders,true));
	add_motivation			(eMotivationNoOrders,	xr_new<CAgentManagerMotivationAction>(goal));
}

void CAgentManagerMotivationPlanner::add_evaluators	()
{
	add_evaluator			(ePropertyOrders		,xr_new<CAgentManagerPropertyEvaluatorConst>(false,"property_order"));
	add_evaluator			(ePropertyItem			,xr_new<CAgentManagerPropertyEvaluatorItem>(object(),"property_item"));
	add_evaluator			(ePropertyEnemy			,xr_new<CAgentManagerPropertyEvaluatorEnemy>(object(),"property_enemy"));
}

void CAgentManagerMotivationPlanner::add_actions		()
{
	CAgentManagerActionBase	*action;

	action					= xr_new<CAgentManagerActionNoOrders>		(object(),"no_orders");
	add_condition			(action,ePropertyOrders,			false);
	add_condition			(action,ePropertyItem,				false);
	add_condition			(action,ePropertyEnemy,				false);
	add_effect				(action,ePropertyOrders,			true);
	add_operator			(eOperatorNoOrders,					action);

	action					= xr_new<CAgentManagerActionGatherItems>	(object(),"gather_items");
	add_condition			(action,ePropertyItem,				true);
	add_condition			(action,ePropertyEnemy,				false);
	add_effect				(action,ePropertyItem,				false);
	add_operator			(eOperatorGatherItem,				action);

	action					= xr_new<CAgentManagerActionKillEnemy>		(object(),"kill_enemy");
	add_condition			(action,ePropertyEnemy,				true);
	add_effect				(action,ePropertyEnemy,				false);
	add_operator			(eOperatorKillEnemy,				action);
}

void CAgentManagerMotivationPlanner::remove_links		(CObject *object)
{
}
