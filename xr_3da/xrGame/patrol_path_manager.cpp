////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_manager.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Patrol path manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patrol_path_manager.h"
#include "ai_script_classes.h"

void CPatrolPathManager::select_point(const Fvector &position, u32 &dest_vertex_id)
{
	VERIFY					(m_path && !m_path->tpaWayPoints.empty());
	u32						temp = u32(-1);
	if (!actual() || (m_curr_point_index >= m_path->tpaWayPoints.size())) {
		switch (m_start_type) {
			case ePatrolStartTypeFirst : {
				temp		= 0;
				break;
			}
			case ePatrolStartTypeLast : {
				temp		= m_path->tpaWayPoints.size() - 1;
				break;
			}
			case ePatrolStartTypeNearest : {
				float				min_dist = flt_max;
				for (u32 i=0, n=m_path->tpaWayPoints.size(); i<n; ++i) {
					float			dist = m_path->tpaWayPoints[i].tWayPoint.distance_to(position);
					if (dist < min_dist) {
						temp		= i;
						min_dist	= dist;
					}
				}
				break;
			}
			case ePatrolStartTypePoint : {
				VERIFY		(m_start_point_index < m_path->tpaWayPoints.size());
				temp		= m_start_point_index;
				break;
			}
			default			: NODEFAULT;
		}
		VERIFY				(temp < m_path->tpaWayPoints.size());
		if (m_prev_point_index >= m_path->tpaWayPoints.size())
			m_prev_point_index	= temp;
		m_curr_point_index	= temp;
		dest_vertex_id		= m_path->tpaWayPoints[m_curr_point_index].dwNodeID;
		m_dest_position		= m_path->tpaWayPoints[m_curr_point_index].tWayPoint;
		m_actuality			= true;
		m_completed			= false;
		return;
	}
	VERIFY					(m_curr_point_index < m_path->tpaWayPoints.size());

	if (m_callback)
		SCRIPT_CALLBACK_EXECUTE_3((*m_callback), dynamic_cast<CGameObject*>(this)->lua_game_object(),u32(CScriptMonster::eActionTypeMovement),m_curr_point_index);

	u32						count = 0;
	float					sum = 0.f;
	for (u32 i=0, n=m_path->tpaWayLinks.size(); i<n; ++i)
		if ((m_path->tpaWayLinks[i].wFrom == m_curr_point_index) && (m_path->tpaWayLinks[i].wTo != m_prev_point_index)) {
			if (!count)
				temp		= i;
			sum				+= m_path->tpaWayLinks[i].fProbability;
			++count;
		}

	if (!count) {
		switch (m_route_type) {
			case ePatrolRouteTypeStop : {
				m_completed	= true;
				return;
			}
			case ePatrolRouteTypeContinue : {
				for (u32 i=0, n=m_path->tpaWayLinks.size(); i<n; ++i)
					if (m_path->tpaWayLinks[i].wFrom == m_curr_point_index) {
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
		float			fChoosed = 0.f;
		if (random() && (count > 1))
			fChoosed	= ::Random.randF(sum);
		sum				= 0.f;
		for (int i=0, n=(int)m_path->tpaWayLinks.size(); i<n; ++i)
			if ((m_path->tpaWayLinks[i].wFrom == m_curr_point_index) && (m_path->tpaWayLinks[i].wTo != m_prev_point_index)) {
				sum		+= m_path->tpaWayLinks[i].fProbability;
				if (sum >= fChoosed) {
					temp = i;
					break;
				}
			}
		VERIFY			(temp < m_path->tpaWayLinks.size());
	}
	
	m_prev_point_index	= m_curr_point_index;
	m_curr_point_index	= m_path->tpaWayLinks[temp].wTo;
	dest_vertex_id		= m_path->tpaWayPoints[m_curr_point_index].dwNodeID;
	m_dest_position		= m_path->tpaWayPoints[m_curr_point_index].tWayPoint;
}