////////////////////////////////////////////////////////////////////////////
//	Module 		: detailed_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Detail path manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "detail_path_manager.h"
#include "ai_space.h"

CDetailPathManager::CDetailPathManager	()
{
	Init					();
}

CDetailPathManager::~CDetailPathManager	()
{
}

void CDetailPathManager::Init			()
{
	m_actual				= false;
	m_failed				= false;
	m_start_position		= Fvector().set(0,0,0);
	m_dest_position			= Fvector().set(0,0,0);
	m_current_travel_point	= u32(-1);
	m_path_type				= eDetailPathTypeSmooth;
}

bool CDetailPathManager::valid			() const
{
	return					(!m_path.empty() && m_path[0].m_position.similar(m_start_position) && m_path[m_path.size() - 1].m_position.similar(m_dest_position));
}

bool CDetailPathManager::valid			(const Fvector &position) const
{
	return					(!!_valid(position));
}

const Fvector &CDetailPathManager::direction()
{
	if ((m_path.size() < 2) || (m_path.size() <= m_current_travel_point + 1))
		return				(Fvector().set(0,0,1));
	
	Fvector					direction;
	direction.sub			(m_path[m_current_travel_point + 1].m_position, m_path[m_current_travel_point].m_position);

	if (direction.magnitude() < EPS_L)
		return				(direction.set(0,0,1));
	else
		return				(direction.normalize());
}

void CDetailPathManager::build_path(const xr_vector<u32> &level_path, u32 intermediate_index, const Fvector &dest_position)
{
	if (valid(m_start_position) && valid(m_dest_position)) {
		switch (m_path_type) {
			case eDetailPathTypeSmooth : {
				build_smooth_path(level_path,intermediate_index,dest_position);
				break;
			}
			case eDetailPathTypeSmoothDodge : {
				build_dodge_path(level_path,intermediate_index,dest_position);
				break;
			}
			case eDetailPathTypeSmoothCriteria : {
				build_criteria_path(level_path,intermediate_index,dest_position);
				break;
			}
		}
		if (valid()) {
			m_actual				= true;
			m_current_travel_point	= 0;
		}
	}
}

void CDetailPathManager::build_criteria_path	(const xr_vector<u32> &level_path, u32 intermediate_index, const Fvector &dest_position)
{
	R_ASSERT				(!level_path.empty());
	STravelPoint			current,next;

	// start point
	m_path.clear			();
	current.m_linear_speed	= 2.15f;//(*m_movement_params.begin()).second.m_linear_speed;
	current.m_angular_speed	= 0.f;//(*m_movement_params.begin()).second.m_angular_speed;
	current.m_position		= m_start_position;
	m_path.push_back		(current);

	// end point
	Fvector					Last = ai().level_graph().vertex_position(level_path.back());

	// segmentation
	CLevelGraph::SContour	Ccur,Cnext;
	ai().level_graph().contour(Ccur,level_path[0]);
	m_segments.clear		();
	for (u32 I=1; I<level_path.size(); ++I) {
		CLevelGraph::SSegment		S;
		ai().level_graph().contour	(Cnext,level_path[I]);
		ai().level_graph().intersect(S,Ccur,Cnext);
		m_segments.push_back(S);
		Ccur				= Cnext;
	}

	// path building
	for (I=0; I<m_segments.size(); ++I)
	{
		// build probe point
		CLevelGraph::SSegment		&S = m_segments[I];
		ai().level_graph().nearest(next.m_position,current.m_position,S.v1,S.v2);

		// select far point
		Fvector				fp	= Last;
		if ((I+1)<m_segments.size())	{
			CLevelGraph::SSegment	&F = m_segments[I+1];
			ai().level_graph().nearest(fp,current.m_position,F.v1,F.v2);
		}

		// try to cast a line from 'current.m_position' to 'fp'
		Fvector	ip;
		if (ai().level_graph().intersect(ip,S.v1,S.v2,current.m_position,fp)) {
			next.m_position.set(ip);
		} else {
			// find nearest point to segment 'current.m_position' to 'fp'
			float			d1 = S.v1.distance_to(fp)+S.v1.distance_to(current.m_position);
			float			d2 = S.v2.distance_to(fp)+S.v2.distance_to(current.m_position);
			if (d1<d2)		next.m_position.set(S.v1);
			else			next.m_position.set(S.v2);
		}

		// record _new point
		if (!next.m_position.similar(m_path.back().m_position))	m_path.push_back(next);
		current				= next;
	}
	next.m_position.set		(Last);
	if (!next.m_position.similar(m_path.back().m_position))	m_path.push_back(next);

	// setup variables
	m_current_travel_point	= 0;
}

bool CDetailPathManager::actual() const
{
	return					(true);
}

void CDetailPathManager::build_dodge_path		(const xr_vector<u32> &level_path, u32 intermediate_index, const Fvector &dest_position)
{
	build_criteria_path		(level_path,intermediate_index,dest_position);
}

void CDetailPathManager::build_smooth_path		(const xr_vector<u32> &level_path, u32 intermediate_index, const Fvector &dest_position)
{
	VERIFY(!level_path.empty());

	if (ai().level_graph().inside(ai().level_graph().vertex(CLevelPathManager::dest_vertex_id()),dest_position))
		dest_position.y = 
			ai().level_graph().vertex_plane_y(
				*ai().level_graph().vertex(level_path),
				dest_position.x,
				dest_position.z
			);
	m_tpaPoints.clear			();
	m_tpaDeviations.clear		();
	m_tpaTravelPath.clear		();
	m_tpaPointNodes.clear		();

	Fvector						tStartPosition = Position();
	u32							dwCurNode = level_vertex_id();
	m_tpaPoints.push_back		(Position());
	m_tpaPointNodes.push_back	(dwCurNode);

	for (u32 i=1; i<=N; ++i)
		if (i<N) {
			if (!ai().level_graph().check_vertex_in_direction(dwCurNode,tStartPosition,m_level_path[i])) {
				if (dwCurNode != m_level_path[i - 1])
					m_tpaPoints.push_back(tStartPosition = ai().level_graph().vertex_position(dwCurNode = m_level_path[--i]));
				else
					m_tpaPoints.push_back(tStartPosition = ai().level_graph().vertex_position(dwCurNode = m_level_path[i]));
				m_tpaPointNodes.push_back(dwCurNode);
			}
		}
		else
			if (ai().level_graph().check_position_in_direction(dwCurNode,tStartPosition,dest_position) == u32(-1)) {
				if (dwCurNode != CLevelPathManager::dest_vertex_id())
					m_tpaPointNodes.push_back(CLevelPathManager::dest_vertex_id());
				m_tpaPoints.push_back(dest_position);
			}
			else {
				dwCurNode = CLevelPathManager::dest_vertex_id();
				if (ai().level_graph().inside(ai().level_graph().vertex(dwCurNode),*tpDestinationPosition)) {
					tpDestinationPosition->y = ai().level_graph().vertex_plane_y(*ai().level_graph().vertex(CLevelPathManager::dest_vertex_id()),tpDestinationPosition->x,tpDestinationPosition->z);
					m_tpaPointNodes.push_back(CLevelPathManager::dest_vertex_id());
					m_tpaPoints.push_back(dest_position);
				}
			}
	
	m_tpaDeviations.resize	(N = (int)m_tpaPoints.size());
	
	CDetailPathManager::m_path.clear();
	
	m_level_path.clear		();
	
	CTravelPoint			T;
	T.m_position.set		(0,0,0);
	for (i=1; i<N; ++i) {
		m_tpaLine.clear();
		m_tpaLine.push_back(m_tpaPoints[i-1]);
		m_tpaLine.push_back(m_tpaPoints[i]);
		ai().level_path().bfCreateStraightPTN_Path(m_tpaPointNodes[i-1],m_tpaPoints[i-1],m_tpaPoints[i],m_tpaTravelPath,m_tpaNodes, i == 1);
		u32 n = (u32)m_tpaTravelPath.size();
		for (u32 j= 0; j<n; ++j) {
			T.P = m_tpaTravelPath[j];
			m_path.push_back(T);
			m_level_path.push_back(m_tpaNodes[j]);
		}
	}
	if (N > 1) {
		m_level_path[m_level_path.size() - 1] = CLevelPathManager::dest_vertex_id();
		if (!m_path.empty() && !m_path[m_path.size() - 1].m_position.similar(dest_position)) {
			if (ai().level_graph().inside(ai().level_graph().vertex(CLevelPathManager::dest_vertex_id()),dest_position) && ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(CLevelPathManager::dest_vertex_id(),T.P,dest_position))) {
				T.P = dest_position;
				m_path.push_back(T);
			}
		}
	}
	else
		if (!m_path.empty() && !m_path[m_path.size() - 1].P.similar(dest_position))
			if (ai().level_graph().inside(ai().level_graph().vertex(CLevelPathManager::dest_vertex_id()),dest_position) && ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(CLevelPathManager::dest_vertex_id(),T.P,dest_position)))
				m_tpaTempPath.push_back(dest_position);
}

