////////////////////////////////////////////////////////////////////////////
//	Module 		: detailed_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Detail path manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "detail_path_manager.h"

CDetailPathManager::CDetailPathManager	()
{
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
}

void CDetailPathManager::build_dodge_path		(const xr_vector<u32> &level_path)
{
}

void CDetailPathManager::build_criteria_path	(const xr_vector<u32> &level_path)
{
//	R_ASSERT				(!level_path.empty());
//	Fvector					current,next;
//
//	// start point
//	m_detail_path.clear		();
//	current.set				(Position());
//	m_detail_path.push_back	(current);
//
//	// end point
//	CLevelGraph&	AI			= ai();
//	Fvector		Last		= AI.level_graph().vertex_position(Nodes.back());
//
//	// segmentation
//	SContour				Ccur,Cnext;
//	UnpackContour			(Ccur,Nodes[0]);
//	Segments.clear			();
//	for (u32 I=1; I<Nodes.size(); I++)
//	{
//		SSegment				S;
//		UnpackContour		(Cnext,Nodes[I]);
//		vfIntersectContours	(S,Ccur,Cnext);
//		Segments.push_back	(S);
//		Ccur				= Cnext;
//	}
//
//	// path building
//	for (I=0; I<Segments.size(); I++)
//	{
//		// build probe point
//		SSegment& S = Segments[I];
//		ClosestPointOnSegment(next.P,current.P,S.v1,S.v2);
//
//		// select far point
//		Fvector fp	= Last;
//		if ((I+1)<Segments.size())	{
//			SSegment& F = Segments[I+1];
//			ClosestPointOnSegment(fp,current.P,F.v1,F.v2);
//		}
//
//		// try to cast a line from 'current.P' to 'fp'
//		Fvector	ip;
//		if (SegmentsIntersect(ip,S.v1,S.v2,current.P,fp)) {
//			next.P.set(ip);
//		} else {
//			// find nearest point to segment 'current.P' to 'fp'
//			float d1 = S.v1.distance_to(fp)+S.v1.distance_to(current.P);
//			float d2 = S.v2.distance_to(fp)+S.v2.distance_to(current.P);
//			if (d1<d2)	next.P.set(S.v1);
//			else		next.P.set(S.v2);
//		}
//
//		// record _new point
//		if (!next.P.similar(TravelPath.back().P))	TravelPath.push_back(next);
//		current				= next;
//	}
//	next.P.set			(Last);
//	if (!next.P.similar(TravelPath.back().P))	TravelPath.push_back(next);
//
//	// setup variables
//	TravelStart				= 0;
//	/**
//	// checking for Y-s
//	for (int i=1; i<TravelPath.size(); i++)
//	if (TravelPath[i].P.y - TravelPath[i - 1].P.y > .5f) {
//	Msg("AI_BuildTravelLine : suspicious Y-point found");
//	}
//	/**/
//	//Engine.Sheduler.Slice	();
}

float CDetailPathManager::speed	()
{
	return	(0);
}
