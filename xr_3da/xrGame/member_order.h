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

class CAI_Stalker;

class CMemberOrder {
protected:
	CAI_Stalker								*m_object;
	AgentManager::EOrderType				m_order_type;
	StalkerDecisionSpace::EWorldProperties	m_goal;
	CSetupAction							m_action;
	bool									m_initialized;

public:
	IC														CMemberOrder	(CAI_Stalker *object);
	IC		bool											initialized		() const;
	IC		CAI_Stalker										*object			() const;
	IC		const CSetupAction								&action			() const;
	IC		const AgentManager::EOrderType					&order_type		() const;
	IC		const StalkerDecisionSpace::EWorldProperties	&goal			() const;
	IC		CSetupAction									&action			();
	IC		void											action			(const CSetupAction	&action);
	IC		void											order_type		(const AgentManager::EOrderType &order_type);
	IC		void											goal			(const StalkerDecisionSpace::EWorldProperties &goal);
};

#include "member_order_inline.h"