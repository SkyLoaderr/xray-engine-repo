#pragma once

IC	bool CMovementManager::actual() const
{
	return				(m_path_actuality);
}

IC	bool CMovementManager::actual_all() const
{
	if (!m_path_actuality)
		return			(false);
	switch (m_path_type) {
		case ePathTypeGamePath : 
			return			(
				CGamePathManager::actual() && 
				CLevelPathManager::actual() &&
				CDetailPathManager::actual()
			);
		case ePathTypeLevelPath :
			return			(
				CLevelPathManager::actual() &&
				CDetailPathManager::actual()
			);
		case ePathTypeEnemySearch :
			return			(
//				CEnemyLocationPredictor::actual() && 
				CLevelPathManager::actual() &&
				CDetailPathManager::actual()
			);
		case ePathTypePatrolPath :
			return			(
				CPatrolPathManager::actual() && 
				CLevelPathManager::actual() &&
				CDetailPathManager::actual()
			);
		case ePathTypeNoPath :
			return			(
				CDetailPathManager::actual()
			);
		default : NODEFAULT;
	}
#ifdef DEBUG
	return					(true);
#endif
}

IC	void CMovementManager::set_path_type(EPathType path_type)
{
	m_path_actuality		= m_path_actuality && (m_path_type == path_type);
	m_path_type				= path_type;
}

IC	void CMovementManager::set_game_dest_vertex	(const ALife::_GRAPH_ID game_vertex_id)
{
	VERIFY					(!CDetailPathManager::m_restricted_object || CDetailPathManager::m_restricted_object->accessible(ai().game_graph().vertex(game_vertex_id)->level_point()));
	CGamePathManager::set_dest_vertex(game_vertex_id);
	m_path_actuality		= m_path_actuality && CGamePathManager::actual();
}

IC	void CMovementManager::set_level_dest_vertex(const u32 level_vertex_id)
{
	VERIFY					(!CDetailPathManager::m_restricted_object || CDetailPathManager::m_restricted_object->accessible(level_vertex_id));
	CLevelPathManager::set_dest_vertex(level_vertex_id);
	m_path_actuality		= m_path_actuality && CLevelPathManager::actual();
}

IC	void CMovementManager::time_start()
{
	m_start_time			= CPU::GetCycleCount();
}

IC	bool CMovementManager::time_over() const
{
	return					(CPU::GetCycleCount() - m_start_time >= m_time_work);
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

IC	MovementManager::EPathType CMovementManager::path_type() const
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

IC	const xr_vector<DetailPathManager::STravelPathPoint>	&CMovementManager::path	() const
{
	return					(CDetailPathManager::path());
}

IC	void CMovementManager::set_body_orientation(const MonsterSpace::SBoneRotation &orientation)
{
	m_body				= orientation;
}

IC	const MonsterSpace::SBoneRotation &CMovementManager::body_orientation() const
{
	return				(m_body);
}

IC	CGraphEngine::CBaseParameters	*CMovementManager::base_game_selector()
{
	return				(m_base_game_selector);
}

IC	CGraphEngine::CBaseParameters	*CMovementManager::base_level_selector()
{
	return				(m_base_level_selector);
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
	return				(CRestrictedObject::accessible(position_or_vertex_id,radius));
}
