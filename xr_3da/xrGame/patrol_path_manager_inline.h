////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_manager_inline.h
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Patrol path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CPatrolPathManager::CPatrolPathManager		()
{
	Init					();
}

IC	CPatrolPathManager::~CPatrolPathManager		()
{
}

IC	void CPatrolPathManager::Init				()
{
	m_path					= 0;
	m_start_type			= ePatrolStartTypeDummy;
	m_route_type			= ePatrolRouteTypeDummy;
	m_actuality				= true;
	m_failed				= false;
	m_completed				= true;
	m_curr_point_index		= u32(-1);
	m_prev_point_index		= u32(-1);
	m_callback				= 0;
}

IC	bool CPatrolPathManager::actual				() const
{
	return					(m_actuality);
}

IC	bool CPatrolPathManager::failed				() const
{
	return					(m_failed);
}

IC	bool CPatrolPathManager::completed			() const
{
	return					(m_completed);
}

IC	bool CPatrolPathManager::random				() const
{
	return					(m_random);
}

IC	const Fvector &CPatrolPathManager::destination_position	() const
{
	VERIFY					(_valid(m_dest_position));
	return					(m_dest_position);
}

IC	void CPatrolPathManager::set_path	(const CLevel::SPath *path, ref_str path_name)
{
	if (m_path == path)
		return;
	m_path					= path;
	m_path_name				= path_name;
	m_actuality				= false;
}

IC	void CPatrolPathManager::set_start_type	(const EPatrolStartType patrol_start_type)
{
	m_actuality				= m_actuality && (m_start_type == patrol_start_type);
	m_start_type			= patrol_start_type;
}

IC	void CPatrolPathManager::set_route_type		(const EPatrolRouteType patrol_route_type)
{
	m_actuality				= m_actuality && (m_route_type == patrol_route_type);
	m_route_type			= patrol_route_type;
}

IC	void CPatrolPathManager::set_random	(bool random)
{
	m_random				= random;
}

IC	void CPatrolPathManager::set_callback(const CScriptMonster::SMemberCallback &callback)
{
	m_callback				= &callback;
}
