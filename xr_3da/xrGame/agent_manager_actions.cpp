////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_actions.cpp
//	Created 	: 25.05.2004
//  Modified 	: 25.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager actions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "agent_manager_actions.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "agent_location_manager.h"
#include "agent_corpse_manager.h"
#include "agent_explosive_manager.h"
#include "agent_enemy_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "sight_action.h"
#include "inventory.h"

//#define TEST

//////////////////////////////////////////////////////////////////////////
// CAgentManagerActionBase
//////////////////////////////////////////////////////////////////////////

CAgentManagerActionBase::CAgentManagerActionBase	(CAgentManager *object, LPCSTR action_name) :
	inherited	(object,action_name)
{
}

void CAgentManagerActionBase::initialize			()
{
}

void CAgentManagerActionBase::finalize				()
{
}

void CAgentManagerActionBase::execute				()
{
}

//////////////////////////////////////////////////////////////////////////
// CAgentManagerActionNoOrders
//////////////////////////////////////////////////////////////////////////

CAgentManagerActionNoOrders::CAgentManagerActionNoOrders	(CAgentManager *object, LPCSTR action_name) :
	inherited		(object,action_name)
{
}

void CAgentManagerActionNoOrders::initialize		()
{
}

void CAgentManagerActionNoOrders::finalize			()
{
	m_object->corpse().clear();
}

void CAgentManagerActionNoOrders::execute			()
{
//	CGraphEngine::CWorldState	goal;
//	goal.add_condition			(CGraphEngine::CWorldProperty(StalkerDecisionSpace::eWorldPropertyEnemy,true));
	CAgentMemberManager::iterator		I = m_object->member().members().begin();
	CAgentMemberManager::iterator		E = m_object->member().members().end();
	for ( ; I != E; ++I) {
#ifndef TEST
		(*I)->order_type			(AgentManager::eOrderTypeNoOrder);
#else
		(*I)->order_type			(AgentManager::eOrderTypeAction);
		(*I)->action				(CSetupAction(0.f,0));
		(*I)->action().movement().set_level_dest_vertex_id((*I)->object().ai_location().level_vertex_id());
#endif

//		(*I).order_type			(AgentManager::eOrderTypeGoal);
//		(*I).goal				(goal);

//		(*I).order_type			(AgentManager::eOrderTypeAction);
//		(*I).action				(CSetupAction(0.f,0));
//		(*I).action().movement().set_level_dest_vertex_id((*I).object().ai_location().level_vertex_id());
//		(*I).action().object	(CObjectAction(MonsterSpace::eObjectActionUse,(*I).object().inventory().item(CLSID_IITEM_BOTTLE)));
//		(*I).action().sight		(CSightAction(smart_cast<CGameObject*>(Level().CurrentEntity()),false,""));
	}
}

//////////////////////////////////////////////////////////////////////////
// CAgentManagerActionGatherItems
//////////////////////////////////////////////////////////////////////////

CAgentManagerActionGatherItems::CAgentManagerActionGatherItems	(CAgentManager *object, LPCSTR action_name) :
	inherited		(object,action_name)
{
}

void CAgentManagerActionGatherItems::initialize		()
{
}

void CAgentManagerActionGatherItems::finalize			()
{
}

void CAgentManagerActionGatherItems::execute			()
{
	CAgentMemberManager::iterator		I = m_object->member().members().begin();
	CAgentMemberManager::iterator		E = m_object->member().members().end();
	for ( ; I != E; ++I)
		(*I)->order_type	(AgentManager::eOrderTypeNoOrder);
}

//////////////////////////////////////////////////////////////////////////
// CAgentManagerActionKillEnemy
//////////////////////////////////////////////////////////////////////////

CAgentManagerActionKillEnemy::CAgentManagerActionKillEnemy	(CAgentManager *object, LPCSTR action_name) :
	inherited		(object,action_name)
{
}

void CAgentManagerActionKillEnemy::initialize		()
{
	m_level_time					= Device.dwTimeGlobal + 10000;
	m_object->location().clear		();
}

void CAgentManagerActionKillEnemy::finalize			()
{
}

void CAgentManagerActionKillEnemy::execute			()
{
	m_object->enemy().distribute_enemies		();
	m_object->explosive().react_on_explosives	();
	m_object->corpse().react_on_member_death	();

	CAgentMemberManager::iterator	I = m_object->member().members().begin();
	CAgentMemberManager::iterator	E = m_object->member().members().end();
	for ( ; I != E; ++I) {
#ifndef TEST
		(*I)->order_type			(AgentManager::eOrderTypeNoOrder);
#else
		if ((*I).object().enemy())
			Msg						("%6d : %s vs %s",Device.dwTimeGlobal,*(*I).object().cName(),*(*I).object().enemy()->cName());
		if (m_level_time >= Device.dwTimeGlobal) {
			(*I).order_type			(AgentManager::eOrderTypeAction);
			(*I).action				(CSetupAction(0.f,0));
			(*I).action().movement().set_level_dest_vertex_id((*I).object().ai_location().level_vertex_id());
			if ((*I).object().enemy())
			(*I).action().sight		(CSightAction((*I).object().enemy(),true));
		}
		else
			(*I).order_type			(AgentManager::eOrderTypeNoOrder);
#endif
	}
}
