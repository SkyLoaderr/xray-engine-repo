////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_restriction.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker movement restriction
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai/stalker/ai_stalker_impl.h"
#include "agent_manager.h"
#include "agent_location_manager.h"

class CAI_Stalker;

class CStalkerMovementRestrictor {
private:
	CAI_Stalker			*m_object;
	const CAgentManager	*m_agent_manager;
	bool				m_use_enemy_info;

public:
	IC					CStalkerMovementRestrictor	(CAI_Stalker *object, bool use_enemy_info);
	IC		bool		operator()					(CCoverPoint *cover) const;
	IC		float		weight						(CCoverPoint *cover) const;
};

#include "stalker_movement_restriction_inline.h"