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
}

struct SPathPoint {
	u32				vertex_id;
	Fvector			position;
	Fvector			direction;
	float			linear_velocity;
	float			angular_velocity; 
};

struct STangentPoint {
	Fvector			point;
	float			angle;
};

struct SCirclePoint :
	public STangentPoint
{
	Fvector			center;
	float			radius;
};

struct STrajectoryPoint :
	public SPathPoint,
	public SCirclePoint
{
};

IC	void adjust_point(const Fvector &source, float yaw, float magnitude, Fvector &dest)
{
	dest.setHP		(yaw,0);
	dest.mul		(magnitude);
	dest.add		(source);
}

IC	void compute_circles(
	STrajectoryPoint &point, 
	SCirclePoint	 *circles
)
{
	point.radius	= point.linear_velocity/point.angular_velocity;
	circles[0].radius = circles[1].radius = point.radius;
	float			yaw, pitch;
	point.direction.getHP(yaw,pitch);
	adjust_point	(point.position,yaw + PI_DIV_2,point.radius,circles[0].center);
	adjust_point	(point.position,yaw - PI_DIV_2,point.radius,circles[1].center);
}

IC	void compute_tangent(
	const SCirclePoint		&start, 
	const SCirclePoint		&dest, 
	STangentPoint			*tangent
)
{
	// distance between circle centers
	float			distance = start.center.distance_to(dest.center);
	// radius difference
	float			r_diff = start.radius - dest.radius;
	// angle between external tangents and circle centers segment
	float			alpha = acosf(r_diff/distance);
	
	Fvector			direction;
	// direction from the first circle to the second one
	direction.sub	(dest.center,start.center);
	float			yaw, pitch;
	direction.getHP	(yaw,pitch);

	float			start_angle = 0.f, dest_angle = 0.f;
	if (r_diff > 0)
		dest_angle	= PI_DIV_2;
	else
		start_angle	= PI_DIV_2;

	// compute external tangents points
	Fvector			tangents[4][2];
	adjust_point	(start.center,yaw + (alpha - start_angle),	start.radius,tangents[0][0]);
	adjust_point	(start.center,yaw - (alpha - start_angle),	start.radius,tangents[1][0]);
	adjust_point	(dest.center,yaw  + (alpha - dest_angle),	dest.radius, tangents[0][1]);
	adjust_point	(dest.center,yaw  - (alpha - dest_angle),	dest.radius, tangents[1][1]);
	
#pragma todo("Check if this tangents are possible, i.e. circles aren't intersecting")
	// angle between internal tangents and circle centers segment
	alpha			= PI_DIV_2 - asinf((start.radius + dest.radius)/distance);

	// compute internal tangents points
	adjust_point	(start.center,yaw + alpha,					start.radius,tangents[2][0]);
	adjust_point	(start.center,yaw - alpha,					start.radius,tangents[3][0]);
	adjust_point	(dest.center,yaw + alpha,					dest.radius, tangents[2][1]);
	adjust_point	(dest.center,yaw - alpha,					dest.radius, tangents[3][1]);

	// choosing valid tangents (it is a single one)
}

u32	 choose_tangent(const STangentPoint	tangents[4][2])
{
	return			(0);
}

void build_trajectory(
	const STrajectoryPoint	&start, 
	const STrajectoryPoint	&dest, 
	const STangentPoint		*tangents,
	const xr_vector<Fvector>&path
)
{
}

bool compute_trajectory(
	STrajectoryPoint &start, 
	STrajectoryPoint &dest, 
	xr_vector<Fvector> &path
)
{
	SCirclePoint	start_circles[2], dest_circles[2];
	compute_circles	(start,start_circles);
	compute_circles	(dest,dest_circles);
	
	STangentPoint	tangent_points[4][2];
	for (u32 i=0; i<2; ++i)
		for (u32 j=0; j<2; ++j)
			compute_tangent(start_circles[i],dest_circles[j],tangent_points[2*i + j]);

	u32				choosen = choose_tangent(tangent_points);
	if (choosen > 4)
		return		(false);

	build_trajectory(start,dest,tangent_points[choosen],path);

	return			(true);
}