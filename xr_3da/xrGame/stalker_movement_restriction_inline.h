////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_restriction_inline.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker movement restriction inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CStalkerMovementRestrictor::CStalkerMovementRestrictor	(CAI_Stalker *object, bool use_enemy_info)
{
	m_object			= object;
	m_agent_manager		= &object->agent_manager();
	m_use_enemy_info	= use_enemy_info;
}

IC	bool CStalkerMovementRestrictor::operator()				(CCoverPoint *cover) const
{
	return				(m_agent_manager->location().suitable(m_object,cover,m_use_enemy_info));
}

IC	float CStalkerMovementRestrictor::weight				(CCoverPoint *cover) const
{
	return				(m_agent_manager->location().danger(cover));
}
