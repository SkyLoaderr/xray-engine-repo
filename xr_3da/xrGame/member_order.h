////////////////////////////////////////////////////////////////////////////
//	Module 		: member_order.h
//	Created 	: 26.05.2004
//  Modified 	: 26.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Member order
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "setup_action.h"
#include "agent_manager_space.h"
#include "stalker_decision_space.h"
#include "graph_engine_space.h"
#include "condition_state.h"

class CAI_Stalker;
class CCoverPoint;
class CExplosive;

class CMemberOrder {
public:
	typedef AgentManager::EOrderType		EOrderType;
	typedef GraphEngineSpace::CWorldState	CWorldState;

public:
	struct CMemberDeathReaction {
		CAI_Stalker				*m_member;
		u32						m_time;
		bool					m_processing;

		IC			CMemberDeathReaction()
		{
			clear				();
		}

		IC	void	clear				()
		{
			m_member			= 0;
			m_time				= 0;
			m_processing		= false;
		}
	};

	struct CGrenadeReaction {
		const CExplosive		*m_grenade;
		const CGameObject		*m_game_object;
		u32						m_time;
		bool					m_processing;

		IC			CGrenadeReaction	()
		{
			clear				();
		}

		IC	void	clear				()
		{
			m_grenade			= 0;
			m_game_object		= 0;
			m_time				= 0;
			m_processing		= false;
		}
	};

protected:
	CAI_Stalker					*m_object;
	mutable CCoverPoint			*m_cover;
	EOrderType					m_order_type;
	CWorldState					m_goal;
	CSetupAction				m_action;
	bool						m_initialized;
	float						m_probability;
	xr_vector<u32>				m_enemies;
	bool						m_processed;
	u32							m_selected_enemy;
	mutable CMemberDeathReaction m_member_death_reaction;
	mutable CGrenadeReaction	m_grenade_reaction;

public:
	IC							CMemberOrder			(CAI_Stalker *object);
	IC		bool				initialized				() const;
	IC		CAI_Stalker			&object					() const;
	IC		const CSetupAction	&action					() const;
	IC		const EOrderType	&order_type				() const;
	IC		const CWorldState	&goal					() const;
	IC		float				probability				() const;
	IC		bool				processed				() const;
	IC		u32					selected_enemy			() const;
	IC		CCoverPoint			*cover					() const;
	IC		CMemberDeathReaction&member_death_reaction	() const;
	IC		CGrenadeReaction	&grenade_reaction		() const;
	IC		CSetupAction		&action					();
	IC		xr_vector<u32>		&enemies				();
	IC		void				cover					(CCoverPoint *object_cover) const;
	IC		void				action					(const CSetupAction	&action);
	IC		void				order_type				(const EOrderType &order_type);
	IC		void				goal					(const CWorldState &goal);
	IC		void				probability				(float probability);
	IC		void				processed				(bool processed);
	IC		void				selected_enemy			(u32 selected_enemy);
};

#include "member_order_inline.h"