////////////////////////////////////////////////////////////////////////////
//	Module 		: enemy_manager_inline.h
//	Created 	: 30.12.2003
//  Modified 	: 30.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Enemy manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CEnemyManager::CEnemyManager		(CCustomMonster *object)
{
	VERIFY						(object);
	m_object					= object;
	m_ignore_monster_threshold	= 1.f;
	m_max_ignore_distance		= 0.f;
	m_ready_to_save				= true;
	m_visible_now				= false;
	m_last_enemy_time			= 0;
}

IC	u32	CEnemyManager::last_enemy_time	() const
{
	return						(m_last_enemy_time);
}

IC	CEnemyManager::USEFULE_CALLBACK &CEnemyManager::useful_callback	()
{
	return						(m_useful_callback);
}
