////////////////////////////////////////////////////////////////////////////
//	Module 		: detailed_path_manager_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Detail path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	bool CDetailPathManager::failed() const
{
	return					(m_failed);
}

IC	bool CDetailPathManager::completed() const
{
	return					(m_path.empty() || m_dest_position.similar(m_path.back().m_position));
}

IC	void CDetailPathManager::set_dest_position(const Fvector &dest_position)
{
	m_dest_position			= dest_position;
}

IC	const xr_vector<CDetailPathManager::STravelPoint> &CDetailPathManager::path() const
{
	return					(m_path);
}

IC	const CDetailPathManager::STravelPoint &CDetailPathManager::curr_travel_point() const
{
	VERIFY					(!m_path.empty() && (m_current_travel_point < m_path.size()));
	return					(m_path[m_current_travel_point]);
}
