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

IC	void CMovementManager::set_game_dest_node	(const ALife::_GRAPH_ID game_vertex_id)
{
	m_path_actuality		= m_path_actuality && (m_game_dest_vertex_id == game_vertex_id);
	m_game_dest_vertex_id	= game_vertex_id;
}

IC	void CMovementManager::set_level_dest_node	(const u32 level_vertex_id)
{
	m_path_actuality		= m_path_actuality && (m_level_dest_vertex_id == level_vertex_id);
	m_level_dest_vertex_id	= level_vertex_id;
}

IC	void CMovementManager::time_start()
{
	m_start_time			= CPU::GetCycleCount();
}

IC	bool CMovementManager::time_over() const
{
	return					(false);//CPU::GetCycleCount() - m_start_time >= m_time_work);
}

IC	void CMovementManager::set_enabled(bool enabled)
{
	m_enabled				= enabled;
}
	
IC	bool CMovementManager::get_enabled() const
{
	return					(m_enabled);
}
