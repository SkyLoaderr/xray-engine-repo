////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_restriction.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker movement_restriction
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai/stalker/ai_stalker_impl.h"
#include "agent_manager.h"
#include "agent_location_manager.h"

class CAI_Stalker;

struct CMovementRestrictor {
	CAI_Stalker			*m_object;
	const CAgentManager	*m_agent_manager;
	bool				m_use_enemy_info;

	IC					CMovementRestrictor	(CAI_Stalker *object, bool use_enemy_info)
	{
		m_object		= object;
		m_agent_manager	= &object->agent_manager();
		m_use_enemy_info= use_enemy_info;
	}

	IC		bool		operator()			(CCoverPoint *cover) const
	{
		return			(m_agent_manager->location().suitable(m_object,cover,m_use_enemy_info));
	}

	IC		float		weight				(CCoverPoint *cover) const
	{
		return			(m_agent_manager->location().danger(cover));
	}
};
