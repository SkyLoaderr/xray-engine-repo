////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager.cpp
//	Created 	: 24.05.2004
//  Modified 	: 24.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "agent_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "agent_manager_actions.h"
#include "agent_manager_motivations.h"
#include "agent_manager_properties.h"

using namespace AgentManager;

#define SECTION "agent_manager"

CAgentManager::CAgentManager		()
{
	reload						(SECTION);
}

CAgentManager::~CAgentManager		()
{
	VERIFY						(m_members.empty());
	shedule_unregister			();
}

float CAgentManager::shedule_Scale	()
{
	return						(.5f);
}

void CAgentManager::shedule_Update	(u32 time_delta)
{
	ISheduled::shedule_Update	(time_delta);
	inherited::update			(time_delta);
}

BOOL CAgentManager::shedule_Ready	()
{
	return						(!m_members.empty());
}

void CAgentManager::reload			(LPCSTR section)
{
	shedule.t_min				= pSettings->r_s32	(section,"schedule_min");
	shedule.t_max				= pSettings->r_s32	(section,"schedule_max");
	shedule_register			();

	inherited::reload			(section);

	clear						();
	add_motivations				();
	add_evaluators				();
	add_actions					();

	inherited::reinit			(this);
}

void CAgentManager::add				(CEntity *member)
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(member);
	if (!stalker)
		return;

	iterator					I = std::find_if(m_members.begin(),m_members.end(), CMemberPredicate(stalker));
	VERIFY						(I == m_members.end());
	
	m_members.push_back			(CMemberOrder(stalker));
}

void CAgentManager::remove			(CEntity *member)
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(member);
	if (!stalker)
		return;

	iterator					I = std::find_if(m_members.begin(),m_members.end(), CMemberPredicate(stalker));
	VERIFY						(I != m_members.end());
	m_members.erase				(I);
}

void CAgentManager::add_motivations	()
{
	CWorldState				goal;

	goal.clear				();
	goal.add_condition		(CWorldProperty(ePropertyOrders,true));
	add_motivation			(eMotivationNoOrders,	xr_new<CAgentManagerMotivationAction>(goal));
}

void CAgentManager::add_evaluators	()
{
	add_evaluator			(ePropertyOrders		,xr_new<CAgentManagerPropertyEvaluatorConst>(false));
	add_evaluator			(ePropertyItem			,xr_new<CAgentManagerPropertyEvaluatorItem>());
	add_evaluator			(ePropertyEnemy			,xr_new<CAgentManagerPropertyEvaluatorEnemy>());
}

void CAgentManager::add_actions		()
{
	CAgentManagerActionBase	*action;

	action					= xr_new<CAgentManagerActionNoOrders>		(this,"no_orders");
	add_condition			(action,ePropertyOrders,			false);
	add_condition			(action,ePropertyItem,				false);
	add_condition			(action,ePropertyEnemy,				false);
	add_effect				(action,ePropertyOrders,			true);
	add_operator			(eOperatorNoOrders,					action);

	action					= xr_new<CAgentManagerActionGatherItems>	(this,"gather_items");
	add_condition			(action,ePropertyItem,				true);
	add_condition			(action,ePropertyEnemy,				false);
	add_effect				(action,ePropertyItem,				false);
	add_operator			(eOperatorGatherItem,				action);

	action					= xr_new<CAgentManagerActionKillEnemy>		(this,"kill_enemy");
	add_condition			(action,ePropertyEnemy,				true);
	add_effect				(action,ePropertyEnemy,				false);
	add_operator			(eOperatorKillEnemy,				action);
}
