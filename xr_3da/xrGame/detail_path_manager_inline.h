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
