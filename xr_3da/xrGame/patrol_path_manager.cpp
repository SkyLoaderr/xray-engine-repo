////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_manager.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Patrol path manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patrol_path_manager.h"

void CPatrolPathManager::select_point(const Fvector &position, u32 &dest_vertex_id)
{
	VERIFY					(Level().m_PatrolPaths.find(*m_path_name) != Level().m_PatrolPaths.end());
	const CLevel::SPath		&patrol_path = (*Level().m_PatrolPaths.find(*m_path_name)).second;
	VERIFY					(!patrol_path.tpaWayPoints.empty());
	u32						temp = u32(-1);
	if (!actual()) {
		switch (m_start_type) {
			case ePatrolStartTypeFirst : {
				temp		= 0;
				break;
			}
			case ePatrolStartTypeLast : {
				temp		= patrol_path.tpaWayPoints.size() - 1;
				break;
			}
			case ePatrolStartTypeNearest : {
				float				min_dist = flt_max;
				for (u32 i=0, n=patrol_path.tpaWayPoints.size(); i<n; ++i) {
					float			dist = patrol_path.tpaWayPoints[i].tWayPoint.distance_to(position);
					if (dist < min_dist) {
						temp		= i;
						min_dist	= dist;
					}
				}
				break;
			}
			default			: NODEFAULT;
		}
		VERIFY				(temp < patrol_path.tpaWayPoints.size());
		m_prev_point_index	= temp;
		m_curr_point_index	= temp;
		dest_vertex_id		= patrol_path.tpaWayPoints[m_curr_point_index].dwNodeID;
		m_dest_position		= patrol_path.tpaWayPoints[m_curr_point_index].tWayPoint;
		m_actuality			= true;
		m_completed			= false;
		return;
	}
	VERIFY					(m_curr_point_index < patrol_path.tpaWayPoints.size());
	
	u32						count = 0;
	for (u32 i=0, n=patrol_path.tpaWayLinks.size(); i<n; ++i)
		if ((patrol_path.tpaWayLinks[i].wFrom == m_curr_point_index) && (patrol_path.tpaWayLinks[i].wTo != m_prev_point_index)) {
			if (!count)
				temp		= i;
			++count;
		}

	if (!count) {
		switch (m_route_type) {
			case ePatrolRouteTypeStop : {
				m_completed	= true;
				return;
			}
			case ePatrolRouteTypeContinue : {
				for (u32 i=0, n=patrol_path.tpaWayLinks.size(); i<n; ++i)
					if (patrol_path.tpaWayLinks[i].wFrom == m_curr_point_index) {
						temp = i;
						break;
					}
				if (temp == u32(-1)) {
					m_completed	= true;
					return;
				}
				break;
			}
			default : NODEFAULT;
		}
	}
	else {
		u32				choosed = count - 1;
		if (random() && (count > 1))
			choosed		= ::Random.randI(count);
		count			= 0;
		for (int i=0, n=(int)patrol_path.tpaWayLinks.size(); i<n; ++i)
			if ((patrol_path.tpaWayLinks[i].wFrom == m_curr_point_index) && (patrol_path.tpaWayLinks[i].wTo != m_prev_point_index))
				if (count == choosed) {
					temp = i;
					break;
				}
				else
					++count;
		VERIFY			(temp < patrol_path.tpaWayLinks.size());
	}
	
	m_prev_point_index	= m_curr_point_index;
	m_curr_point_index	= patrol_path.tpaWayLinks[temp].wTo;
	dest_vertex_id		= patrol_path.tpaWayPoints[m_curr_point_index].dwNodeID;
	m_dest_position		= patrol_path.tpaWayPoints[m_curr_point_index].tWayPoint;
}