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
// CAgentManagerActionIdle
//////////////////////////////////////////////////////////////////////////

CAgentManagerActionIdle::CAgentManagerActionIdle	(CAgentManager *object, LPCSTR action_name) :
	inherited		(object,action_name)
{
}

void CAgentManagerActionIdle::initialize			()
{
}

void CAgentManagerActionIdle::finalize				()
{
}

void CAgentManagerActionIdle::execute				()
{
}
