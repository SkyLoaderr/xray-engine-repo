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
	VERIFY					(m_object);
	m_enemies.reserve		(16);
	m_cover					= 0;
}

IC	bool CMemberOrder::initialized				() const
{
	return			(m_initialized);
}

IC	CAI_Stalker &CMemberOrder::object			() const
{
	VERIFY			(m_object);
	return			(*m_object);
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

IC	const GraphEngineSpace::CWorldState &CMemberOrder::goal	() const
{
	VERIFY			(m_initialized);
	VERIFY			(m_order_type == AgentManager::eOrderTypeGoal);
	return			(m_goal);
}

IC	void CMemberOrder::goal						(const GraphEngineSpace::CWorldState &goal)
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

IC	xr_vector<u32> &CMemberOrder::enemies		()
{
	return			(m_enemies);
}

IC	bool CMemberOrder::processed				() const
{
	return			(m_processed);
}

IC	void CMemberOrder::processed				(bool processed)
{
	m_processed		= processed;
}

IC	u32	 CMemberOrder::selected_enemy			() const
{
	return			(m_selected_enemy);
}

IC	void CMemberOrder::selected_enemy			(u32 selected_enemy)
{
	m_selected_enemy = selected_enemy;
}

IC	void CMemberOrder::cover					(CCoverPoint *object_cover) const
{
	m_cover			= object_cover;
}

IC	CCoverPoint	*CMemberOrder::cover			() const
{
	return			(m_cover);
}

IC	CMemberOrder::CMemberDeathReaction &CMemberOrder::member_death_reaction	() const
{
	return			(m_member_death_reaction);
}

IC	CMemberOrder::CGrenadeReaction &CMemberOrder::grenade_reaction			() const
{
	return			(m_grenade_reaction);
}
