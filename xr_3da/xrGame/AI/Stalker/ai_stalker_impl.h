////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_impl.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker" (inline functions implementation)
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../level.h"
#include "../../seniority_hierarchy_holder.h"
#include "../../team_hierarchy_holder.h"
#include "../../squad_hierarchy_holder.h"
#include "../../group_hierarchy_holder.h"

IC	CAgentManager &CAI_Stalker::agent_manager	() const
{
	return	(Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).agent_manager());
}

