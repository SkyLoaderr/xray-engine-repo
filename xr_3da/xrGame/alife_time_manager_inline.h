////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_time_manager_inline.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife time manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	void			CALifeTimeManager::set_time_factor		(float time_factor)
{
	m_game_time					= game_time();
	m_start_time				= Device.TimerAsync();
	m_time_factor				= time_factor;
};

IC	ALife::_TIME_ID	CALifeTimeManager::game_time			() const
{
	return						(m_game_time + iFloor(m_time_factor*float(Device.TimerAsync() - m_start_time)));
};

IC	float			CALifeTimeManager::time_factor	() const
{
	return						(m_time_factor);
}

IC	float			CALifeTimeManager::normal_time_factor	() const
{
	return						(m_normal_time_factor);
}

IC	ALife::_TIME_ID	CALifeTimeManager::next_surge_time		() const
{
	return						(m_next_surge_time);
}

IC	ALife::_TIME_ID	CALifeTimeManager::last_surge_time		() const
{
	return						(m_last_surge_time);
}

IC	void CALifeTimeManager::next_surge_time					(ALife::_TIME_ID next_surge_time)
{
	m_next_surge_time			= next_surge_time + m_surge_interval;
}

IC	void CALifeTimeManager::last_surge_time					(ALife::_TIME_ID last_surge_time)
{
	m_last_surge_time			= last_surge_time;
}