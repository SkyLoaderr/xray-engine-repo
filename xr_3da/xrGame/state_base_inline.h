////////////////////////////////////////////////////////////////////////////
//	Module 		: state_base_inline.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Base state inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	u32	CStateBase::start_level_time			() const
{
	return				(m_start_level_time);
}

IC	ALife::_TIME_ID	CStateBase::start_game_time	() const
{
	return				(m_start_game_time);
}

IC	u32	CStateBase::inertia_time				() const
{
	return				(m_inertia_time);
}

IC	void CStateBase::set_inertia_time			(u32 inertia_time)
{
	m_inertia_time		= inertia_time;
}
