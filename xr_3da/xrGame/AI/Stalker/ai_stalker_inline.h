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
	return	(TRUE);
}

IC	const CAgentManager	&CAI_Stalker::agent_manager	() const
{
	return	(Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).agent_manager());
}

IC	CStalkerAnimationManager &CAI_Stalker::animation_manager() const
{
	VERIFY	(m_animation_manager);
	return	(*m_animation_manager);
}

IC	LPCSTR CAI_Stalker::Name						() const
{
	return	(CInventoryOwner::Name());
}

IC	float CAI_Stalker::panic_threshold				() const
{
	return	(m_panic_threshold);
}
