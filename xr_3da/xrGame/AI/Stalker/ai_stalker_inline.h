////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_inline.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker" (inline functions)
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	BOOL CAI_Stalker::UsedAI_Locations				()
{
	return				(TRUE);
}

IC	const CAgentManager	&CAI_Stalker::agent_manager	() const
{
	return				(Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()].agent_manager());
}
