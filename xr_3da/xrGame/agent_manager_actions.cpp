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
}

void CAgentManagerActionNoOrders::execute			()
{
	CAgentManager::iterator		I = m_object->members().begin();
	CAgentManager::iterator		E = m_object->members().end();
	for ( ; I != E; ++I)
		(*I).order_type			(AgentManager::eOrderTypeNoOrder);
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
	CAgentManager::iterator		I = m_object->members().begin();
	CAgentManager::iterator		E = m_object->members().end();
	for ( ; I != E; ++I)
		(*I).order_type			(AgentManager::eOrderTypeNoOrder);
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
}

void CAgentManagerActionKillEnemy::finalize			()
{
}

void CAgentManagerActionKillEnemy::execute			()
{
	CAgentManager::iterator		I = m_object->members().begin();
	CAgentManager::iterator		E = m_object->members().end();
	for ( ; I != E; ++I)
		(*I).order_type			(AgentManager::eOrderTypeNoOrder);
}
