#pragma once

IC	bool CMovementManager::actual() const
{
	return					(m_path_actuality);
}

IC	void CMovementManager::set_path_type(EPathType path_type)
{
	m_path_actuality		= m_path_actuality && (m_path_type == path_type);
	m_path_type				= path_type;
}

IC	void CMovementManager::set_game_dest_vertex	(const ALife::_GRAPH_ID game_vertex_id)
{
	CGamePathManager::set_dest_vertex(game_vertex_id);
	m_path_actuality		= m_path_actuality && CGamePathManager::actual();
}

IC	void CMovementManager::set_level_dest_vertex(const u32 level_vertex_id)
{
	CLevelPathManager::set_dest_vertex(level_vertex_id);
	m_path_actuality		= m_path_actuality && CLevelPathManager::actual();
}

IC	void CMovementManager::time_start()
{
	m_start_time			= CPU::GetCycleCount();
}

IC	bool CMovementManager::time_over() const
{
//	return					(CPU::GetCycleCount() - m_start_time >= m_time_work);
	return					(false);
}

IC	void CMovementManager::enable_movement(bool enabled)
{
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

IC	CMovementManager::EPathType CMovementManager::path_type() const
{
	VERIFY					(m_path_type != ePathTypeDummy);
	return					(m_path_type);
}

IC	ALife::_GRAPH_ID CMovementManager::game_dest_vertex_id() const
{
	return					(ALife::_GRAPH_ID(CGamePathManager::dest_vertex_id()));
}

IC	u32	 CMovementManager::level_dest_vertex_id() const
{
	return					(CLevelPathManager::dest_vertex_id());
}

IC	float CMovementManager::desirable_speed		() const
{
	return					(m_desirable_speed);
}

IC	void CMovementManager::set_desirable_speed	(float speed)
{
	m_desirable_speed		= speed;
}