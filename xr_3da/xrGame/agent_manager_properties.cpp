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
#include "agent_manager_space.h"
#include "ai/stalker/ai_stalker.h"

//////////////////////////////////////////////////////////////////////////
// CAgentManagerPropertyEvaluatorGlobal
//////////////////////////////////////////////////////////////////////////

CAgentManagerPropertyEvaluatorItem::_value_type CAgentManagerPropertyEvaluatorItem::evaluate	()
{
	CAgentManager::iterator	I = m_object->members().begin();
	CAgentManager::iterator	E = m_object->members().end();
	for ( ; I != E; ++I)
		if ((*I).object()->item())
			return			(true);
	return					(false);
}

//////////////////////////////////////////////////////////////////////////
// CAgentManagerPropertyEvaluatorEnemy
//////////////////////////////////////////////////////////////////////////

CAgentManagerPropertyEvaluatorEnemy::_value_type CAgentManagerPropertyEvaluatorEnemy::evaluate	()
{
	CAgentManager::iterator	I = m_object->members().begin();
	CAgentManager::iterator	E = m_object->members().end();
	for ( ; I != E; ++I)
		if ((*I).object()->enemy())
			return			(true);
	return					(false);
}
