////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	bool CMovementManager::actual() const
{
	return				(m_path_actuality);
}

IC	void CMovementManager::set_path_type(EPathType path_type)
{
	m_path_actuality		= m_path_actuality && (m_path_type == path_type);
	m_path_type				= path_type;
}

IC	void CMovementManager::time_start()
{
	m_start_time			= CPU::GetCycleCount();
}

IC	bool CMovementManager::time_over() const
{
	return					(!m_build_at_once && (CPU::GetCycleCount() - m_start_time >= m_time_work));
}

IC	void CMovementManager::enable_movement(bool enabled)
{
	m_path_actuality		= m_path_actuality && (m_enabled == enabled);
	m_enabled				= enabled;
}
	
IC	bool CMovementManager::enabled() const
{
	return					(m_enabled);
}

IC	float CMovementManager::speed() const
{
	return					(m_speed);
}

IC	bool CMovementManager::path_completed() const
{
	return					((m_path_state == ePathStatePathCompleted) && actual());
}

IC	void CMovementManager::use_selector_path	(bool selector_path_usage)
{
	m_selector_path_usage	= selector_path_usage;
}

IC	bool CMovementManager::selector_path_used	() const
{
	return					(m_selector_path_usage);
}

IC	float CMovementManager::old_desirable_speed		() const
{
	return					(m_old_desirable_speed);
}

IC	void CMovementManager::set_desirable_speed		(float speed)
{
	m_old_desirable_speed	= speed;
}

IC	void CMovementManager::set_body_orientation(const CBoneRotation &orientation)
{
	m_body				= orientation;
}

IC	const CMovementManager::CBoneRotation &CMovementManager::body_orientation() const
{
	return				(m_body);
}

IC	void CMovementManager::set_refresh_rate		(u32 refresh_rate)
{
	m_refresh_rate		= refresh_rate;
}

IC	u32	 CMovementManager::refresh_rate			() const
{
	return				(m_refresh_rate);
}

template <typename T>
IC	bool CMovementManager::accessible			(T position_or_vertex_id, float radius) const
{
	return				(restrictions().accessible(position_or_vertex_id,radius));
}

IC	void CMovementManager::extrapolate_path		(bool value)
{
	m_path_actuality		= m_path_actuality && (value == m_extrapolate_path);
	m_extrapolate_path		= value;
}

IC	bool CMovementManager::extrapolate_path		() const
{
	return					(m_extrapolate_path);
}

IC	void CMovementManager::set_build_path_at_once()
{
	m_build_at_once			= true;
}

IC	CMovementManager::CBaseParameters	*CMovementManager::base_game_selector() const
{
	return					(m_base_game_selector);
}

IC	CMovementManager::CBaseParameters	*CMovementManager::base_level_selector() const
{
	return					(m_base_level_selector);
}

IC	CMovementManager::CGameLocationSelector		&CMovementManager::game_location_selector	() const
{
	VERIFY					(m_game_location_selector);
	return					(*m_game_location_selector);
}

IC	CMovementManager::CGamePathManager			&CMovementManager::game_path_manager		() const
{
	VERIFY					(m_game_path_manager);
	return					(*m_game_path_manager);
}

IC	CMovementManager::CLevelLocationSelector	&CMovementManager::level_location_selector	() const
{
	VERIFY					(m_level_location_selector);
	return					(*m_level_location_selector);
}

IC	CMovementManager::CLevelPathManager			&CMovementManager::level_path_manager		() const
{
	VERIFY					(m_level_path_manager);
	return					(*m_level_path_manager);
}

IC	CDetailPathManager		&CMovementManager::detail_path_manager		() const
{
	VERIFY					(m_detail_path_manager);
	return					(*m_detail_path_manager);
}

IC	CPatrolPathManager		&CMovementManager::patrol_path_manager		() const
{
	VERIFY					(m_patrol_path_manager);
	return					(*m_patrol_path_manager);
}

IC	CEnemyLocationPredictor	&CMovementManager::enemy_location_predictor	() const
{
	VERIFY					(m_enemy_location_predictor);
	return					(*m_enemy_location_predictor);
}

IC	CRestrictedObject &CMovementManager::restrictions					() const
{
	VERIFY					(m_restricted_object);
	return					(*m_restricted_object);
}

IC	CSelectorManager &CMovementManager::selectors						() const
{
	VERIFY					(m_selector_manager);
	return					(*m_selector_manager);
}

IC	CLocationManager &CMovementManager::locations						() const
{
	VERIFY					(m_location_manager);
	return					(*m_location_manager);
}

IC	CCustomMonster &CMovementManager::object							() const
{
	VERIFY					(m_object);
	return					(*m_object);
}
