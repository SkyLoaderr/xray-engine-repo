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

//////////////////////////////////////////////////////////////////////////
// CAgentManagerPropertyEvaluatorGlobal
//////////////////////////////////////////////////////////////////////////

CAgentManagerPropertyEvaluatorGlobal::_value_type CAgentManagerPropertyEvaluatorGlobal::evaluate	()
{
	return			(false);
}
