////////////////////////////////////////////////////////////////////////////
//	Module 		: member_order_inline.h
//	Created 	: 26.05.2004
//  Modified 	: 26.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Member order inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CMemberOrder::CMemberOrder					(CAI_Stalker *object) :
	m_object		(object),
	m_initialized	(true),
	m_order_type	(AgentManager::eOrderTypeNoOrder),
	m_action		(0.f,0)
{
	VERIFY			(m_object);
}

IC	bool CMemberOrder::initialized				() const
{
	return			(m_initialized);
}

IC	CAI_Stalker *CMemberOrder::object			() const
{
	return			(m_object);
}

IC	const CSetupAction &CMemberOrder::action	() const
{
	VERIFY			(initialized());
	return			(m_action);
}

IC	CSetupAction &CMemberOrder::action			()
{
	VERIFY			(initialized());
	return			(m_action);
}

IC	void CMemberOrder::action					(const CSetupAction	&action)
{
	m_order_type	= AgentManager::eOrderTypeAction;
	m_action		= action;
}

IC	const AgentManager::EOrderType &CMemberOrder::order_type	() const
{
	VERIFY			(m_initialized);
	return			(m_order_type);
}

IC	void CMemberOrder::order_type				(const AgentManager::EOrderType &order_type)
{
	m_order_type	= order_type;
}

IC	const CGraphEngine::CWorldState &CMemberOrder::goal	() const
{
	VERIFY			(m_initialized);
	VERIFY			(m_order_type == AgentManager::eOrderTypeGoal);
	return			(m_goal);
}

IC	void CMemberOrder::goal						(const CGraphEngine::CWorldState &goal)
{
	m_order_type	= AgentManager::eOrderTypeGoal;
	m_goal			= goal;
}

IC	float CMemberOrder::probability				() const
{
	return			(m_probability);
}

IC	void CMemberOrder::probability				(float probability)
{
	m_probability	= probability;
}