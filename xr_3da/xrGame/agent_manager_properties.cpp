////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_properties.cpp
//	Created 	: 25.05.2004
//  Modified 	: 25.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager properties
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "agent_manager_properties.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "agent_manager_space.h"
#include "ai/stalker/ai_stalker.h"
#include "memory_manager.h"
#include "item_manager.h"
#include "enemy_manager.h"

//////////////////////////////////////////////////////////////////////////
// CAgentManagerPropertyEvaluatorGlobal
//////////////////////////////////////////////////////////////////////////

CAgentManagerPropertyEvaluatorItem::_value_type CAgentManagerPropertyEvaluatorItem::evaluate	()
{
	CAgentMemberManager::iterator	I = m_object->member().members().begin();
	CAgentMemberManager::iterator	E = m_object->member().members().end();
	for ( ; I != E; ++I)
		if ((*I).object().memory().item().selected())
			return			(true);
	return					(false);
}

//////////////////////////////////////////////////////////////////////////
// CAgentManagerPropertyEvaluatorEnemy
//////////////////////////////////////////////////////////////////////////

CAgentManagerPropertyEvaluatorEnemy::_value_type CAgentManagerPropertyEvaluatorEnemy::evaluate	()
{
	CAgentMemberManager::iterator	I = m_object->member().members().begin();
	CAgentMemberManager::iterator	E = m_object->member().members().end();
	for ( ; I != E; ++I)
		if ((*I).object().memory().enemy().selected())
			return			(true);
	return					(false);
}
