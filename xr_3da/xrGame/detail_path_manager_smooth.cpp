////////////////////////////////////////////////////////////////////////////
//	Module 		: detailed_path_manager_criteria.h
//	Created 	: 04.12.2003
//  Modified 	: 04.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Detail path manager criteria path builder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "detail_path_manager.h"
#include "ai_space.h"

void CDetailPathManager::build_smooth_path		(const xr_vector<u32> &level_path, u32 intermediate_index)
{
//	build_straight_path			(level_path,intermediate_index);

	VERIFY						(!level_path.empty());
	VERIFY						(level_path.size() > intermediate_index);

	m_tpaPoints.clear			();
	m_tpaDeviations.clear		();
	m_tpaTravelPath.clear		();
	m_tpaPointNodes.clear		();

	Fvector						tStartPosition = m_start_position;
	u32							dwCurNode = level_vertex_id(), N = level_path.size(), dest_vertex_id = level_path[intermediate_index];
	m_tpaPoints.push_back		(tStartPosition);
	m_tpaPointNodes.push_back	(dwCurNode);

	for (u32 i=1; i<=N; ++i)
		if (i<N) {
			if (!ai().level_graph().check_vertex_in_direction(dwCurNode,tStartPosition,level_path[i])) {
				if (dwCurNode != level_path[i - 1])
					m_tpaPoints.push_back(tStartPosition = ai().level_graph().vertex_position(dwCurNode = level_path[--i]));
				else
					m_tpaPoints.push_back(tStartPosition = ai().level_graph().vertex_position(dwCurNode = level_path[i]));
				m_tpaPointNodes.push_back(dwCurNode);
			}
		}
		else
			if (ai().level_graph().check_position_in_direction(dwCurNode,tStartPosition,dest_position()) == u32(-1)) {
				if (dwCurNode != dest_vertex_id)
					m_tpaPointNodes.push_back(dest_vertex_id);
				m_tpaPoints.push_back(dest_position());
			}
			else {
				if (dwCurNode != dest_vertex_id) {
					dwCurNode = dest_vertex_id;
					m_tpaPointNodes.push_back(dest_vertex_id);
				}
				m_tpaPoints.push_back(dest_position());
			}
	
	m_tpaDeviations.resize	(N = (int)m_tpaPoints.size());
	
	m_path.clear			();
	
	STravelPoint			T;
	T.m_position.set		(0,0,0);
	for (i=1; i<N; ++i) {
		m_tpaLine.clear();
		m_tpaLine.push_back(m_tpaPoints[i-1]);
		m_tpaLine.push_back(m_tpaPoints[i]);
		ai().level_graph().create_straight_PTN_path(m_tpaPointNodes[i-1],m_tpaPoints[i-1],m_tpaPoints[i],m_tpaTravelPath,m_tpaNodes, i == 1);
		u32 n = (u32)m_tpaTravelPath.size();
		for (u32 j= 0; j<n; ++j) {
			T.m_position = m_tpaTravelPath[j];
			m_path.push_back(T);
		}
	}
	if (N > 1) {
		if (!m_path.empty() && !m_path[m_path.size() - 1].m_position.similar(dest_position())) {
			if (ai().level_graph().inside(ai().level_graph().vertex(dest_vertex_id),dest_position()) && ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(dest_vertex_id,T.m_position,dest_position()))) {
				T.m_position = dest_position();
				m_path.push_back(T);
			}
		}
	}

	m_current_travel_point	= 0;
}