////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_inline.h
//	Created 	: 24.05.2004
//  Modified 	: 24.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const CAgentManager::MEMBER_STORAGE	&CAgentManager::members	() const
{
	return			(m_members);
}

IC	CAgentManager::MEMBER_STORAGE	&CAgentManager::members	()
{
	return			(m_members);
}

IC	const CSetupAction &CAgentManager::action	(CAI_Stalker *object) const
{
	return			(member(object).action());
}

IC	const CMemberOrder &CAgentManager::member	(CAI_Stalker *object) const
{
	const_iterator	I = std::find_if(members().begin(), members().end(), CMemberPredicate(object));
	VERIFY			(I != members().end());
	return			(*I);
}
