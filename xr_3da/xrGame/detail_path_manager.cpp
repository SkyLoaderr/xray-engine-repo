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
	init						();
}

CDetailPathManager::~CDetailPathManager	()
{
}

void CDetailPathManager::init			()
{
	m_detail_path_actual		= true;
	m_detail_start_position		= Fvector();
	m_detail_dest_position		= Fvector().set(0,0,0);
	m_detail_cur_point_index	= u32(-1);
	m_detail_path_type			= eDetailPathTypeSmooth;
}

bool CDetailPathManager::valid			() const
{
	return				(!m_detail_path.empty() && m_detail_path[0].m_position.similar(m_detail_start_position) && m_detail_path[m_detail_path.size() - 1].m_position.similar(m_detail_dest_position));
}

bool CDetailPathManager::valid			(const Fvector &position) const
{
	return				(!!_valid(position));
}

const Fvector &CDetailPathManager::direction()
{
	if ((m_detail_path.size() < 2) || (m_detail_path.size() <= m_detail_cur_point_index + 1))
		return		(Fvector().set(0,0,1));
	
	Fvector			direction;
	direction.sub	(m_detail_path[m_detail_cur_point_index + 1].m_position, m_detail_path[m_detail_cur_point_index].m_position);

	if (direction.magnitude() < EPS_L)
		return		(direction.set(0,0,1));
	else
		return		(direction.normalize());
}

void CDetailPathManager::build_detail_path(const xr_vector<u32> &level_path)
{
	if (!m_detail_path_actual && valid(m_detail_start_position) && valid(m_detail_dest_position)) {
		switch (m_detail_path_type) {
			case eDetailPathTypeSmooth : {
				build_smooth_path(level_path);
				break;
			}
			case eDetailPathTypeSmoothDodge : {
				build_dodge_path(level_path);
				break;
			}
			case eDetailPathTypeSmoothCriteria : {
				build_criteria_path(level_path);
				break;
			}
		}
		if (valid()) {
			m_detail_path_actual		= true;
			m_detail_cur_point_index	= 0;
		}
	}
}

void CDetailPathManager::build_smooth_path		(const xr_vector<u32> &level_path)
{
	build_criteria_path		(level_path);
}

void CDetailPathManager::build_dodge_path		(const xr_vector<u32> &level_path)
{
	build_criteria_path		(level_path);
}

void CDetailPathManager::build_criteria_path	(const xr_vector<u32> &level_path)
{
	R_ASSERT				(!level_path.empty());
	STravelPoint			current,next;

	// start point
	m_detail_path.clear		();
	current.m_linear_speed	= (*m_movement_params.begin()).second.m_linear_speed;
	current.m_angular_speed	= (*m_movement_params.begin()).second.m_angular_speed;
	current.m_position		= m_detail_start_position;
	m_detail_path.push_back	(current);

	// end point
	Fvector					Last = ai().level_graph().vertex_position(level_path.back());

	// segmentation
	CLevelGraph::SContour	Ccur,Cnext;
	ai().level_graph().contour(Ccur,level_path[0]);
	m_segments.clear			();
	for (u32 I=1; I<level_path.size(); I++) {
		CLevelGraph::SSegment		S;
		ai().level_graph().contour	(Cnext,level_path[I]);
		ai().level_graph().intersect(S,Ccur,Cnext);
		m_segments.push_back		(S);
		Ccur						= Cnext;
	}

	// path building
	for (I=0; I<m_segments.size(); I++)
	{
		// build probe point
		CLevelGraph::SSegment		&S = m_segments[I];
		ai().level_graph().nearest(next.m_position,current.m_position,S.v1,S.v2);

		// select far point
		Fvector fp	= Last;
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
			float d1 = S.v1.distance_to(fp)+S.v1.distance_to(current.m_position);
			float d2 = S.v2.distance_to(fp)+S.v2.distance_to(current.m_position);
			if (d1<d2)	next.m_position.set(S.v1);
			else		next.m_position.set(S.v2);
		}

		// record _new point
		if (!next.m_position.similar(m_detail_path.back().m_position))	m_detail_path.push_back(next);
		current				= next;
	}
	next.m_position.set			(Last);
	if (!next.m_position.similar(m_detail_path.back().m_position))	m_detail_path.push_back(next);

	// setup variables
	m_detail_cur_point_index	= 0;
}

float CDetailPathManager::speed	()
{
	return				(0.f);
}

bool CDetailPathManager::detail_path_actual() const
{
	return				(true);
}

bool CDetailPathManager::detail_path_completed() const
{
	return				(true);
}
