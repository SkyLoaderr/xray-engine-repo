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
	u32				start_vertex_id;
};

struct SCirclePoint {
	Fvector			center;
	float			radius;
	Fvector			point;
	float			angle;
};

struct STrajectoryPoint :
	public SPathPoint,
	public SCirclePoint
{
};

IC	void adjust_point(
	const Fvector		&source, 
	float				yaw, 
	float				magnitude, 
	Fvector				&dest
)
{
	dest.setHP			(yaw,0);
	dest.mul			(magnitude);
	dest.add			(source);
}

IC	void assign_angle(
	float				&angle, 
	const float			start_yaw, 
	const float			dest_yaw, 
	const bool			positive
)
{
	if (positive)
		if (dest_yaw >= start_yaw)
			angle		= dest_yaw - start_yaw;
		else
			angle		= PI_MUL_2 - (dest_yaw - start_yaw);
	else
		if (dest_yaw <= start_yaw)
			angle		= dest_yaw - start_yaw;
		else
			angle		= dest_yaw - start_yaw - PI_MUL_2;
}

IC	void compute_circles(
	STrajectoryPoint	&point, 
	SCirclePoint		*circles
)
{
	point.radius		= point.linear_velocity/point.angular_velocity;
	circles[0].radius	= circles[1].radius = point.radius;
	float				yaw, pitch;
	point.direction.getHP(yaw,pitch);
	adjust_point		(point.position,yaw + PI_DIV_2,point.radius,circles[0].center);
	adjust_point		(point.position,yaw - PI_DIV_2,point.radius,circles[1].center);
}

IC	float cross_product_2D_y(
	const Fvector &point1,
	const Fvector &point2
)
{
	return				(point1.z*point2.x - point1.x*point2.z);
}

IC	bool compute_tangent(
	const STrajectoryPoint	&start, 
	const SCirclePoint		&start_circle, 
	const STrajectoryPoint	&dest, 
	const SCirclePoint		&dest_circle, 
	SCirclePoint			*tangents
)
{
#pragma todo("Dima to Dima : If this will be a slow down, optimize it by using just a few square roots instead of arctangents and arccosinuses")
	float				start_cp, dest_cp, distance, alpha, start_yaw, dest_yaw, pitch, yaw;
	Fvector				direction, temp;
	
	// computing 2D cross product for start point
	direction.sub		(start.point,start_circle.center);
	direction.getHP		(start_yaw,pitch);
	start_cp			= cross_product_2D_y(start.point,direction);
	
	// computing 2D cross product for dest point
	direction.sub		(dest.point,dest_circle.center);
	direction.getHP		(dest_yaw,pitch);
	dest_cp				= cross_product_2D_y(dest.point,direction);

	if (start_cp*dest_cp > 0) {
		// so, our tangents are outside
		// distance between circle centers
		// radius difference
		float			r_diff = start_circle.radius - dest_circle.radius;
		if (fis_zero(r_diff,EPS_L))
			return		(false);

		distance		= start_circle.center.distance_to_xz(dest_circle.center);

		// angle between external tangents and circle centers segment
		alpha			= acosf(r_diff/distance);
	}
	else {
		distance		= start_circle.center.distance_to_xz(dest_circle.center);
		// so, our tangents are inside (crossing)
		if (start_circle.radius + dest_circle.radius > distance)
			return		(false);
	
		// angle between internal tangents and circle centers segment
		alpha			= acosf((start_circle.radius + dest_circle.radius)/distance);
	}

	tangents[0]			= start_circle;
	tangents[1]			= dest_circle;
	start_yaw			= angle_normalize(start_yaw);
	dest_yaw			= angle_normalize(dest_yaw);

	// direction from the first circle to the second one
	direction.sub		(dest_circle.center,start_circle.center);
	direction.getHP		(yaw,pitch);

	// compute external tangent points
	adjust_point		(start_circle.center,yaw + alpha,	start_circle.radius,tangents[0].point);
	adjust_point		(dest_circle.center,yaw  + alpha,	dest_circle.radius, tangents[1].point);

	direction.sub		(tangents[1].point,tangents[0].point);
	temp.sub			(tangents[0].point,start_circle.center);
	float				tangent_cp = cross_product_2D_y(temp,direction);
	if (start_cp*tangent_cp > 0) {
		assign_angle	(tangents[0].angle,start_yaw,angle_normalize(yaw + alpha),start_cp > 0);
		assign_angle	(tangents[1].angle,dest_yaw, angle_normalize(yaw + alpha),dest_cp  > 0);
		return			(true);
	}

	// compute external tangent points
	adjust_point		(start_circle.center,yaw - alpha,	start_circle.radius,tangents[0].point);
	adjust_point		(dest_circle.center,yaw  - alpha,	dest_circle.radius, tangents[1].point);

	return				(true);
}

bool choose_tangent(
	STrajectoryPoint	&start, 
	STrajectoryPoint	&dest, 
	const SCirclePoint	tangents[4][2], 
	const u32			tangent_count
)
{
	float				min_distance = flt_max;
	u32					choosen = u32(-1);
	for (u32 i=0; i<tangent_count; ++i) {
		float			trajectory_length = 
			_abs(tangents[i][0].angle)*tangents[i][0].radius + 
			_abs(tangents[i][1].angle)*tangents[i][1].radius + 
			tangents[i][0].point.distance_to_xz(tangents[i][1].point);
		if (trajectory_length < min_distance) {
			min_distance = trajectory_length;
			choosen		= i;
		}
	}
	if (choosen > tangent_count)
		return			(false);

	SCirclePoint		&_start = start;
	SCirclePoint		&_dest  = dest;
	_start				= tangents[choosen][0];
	_dest				= tangents[choosen][1];

	return				(true);
}

bool build_circle_trajectory(
	const STrajectoryPoint	&position, 
	xr_vector<Fvector>		&path,
	bool					start_point
)
{
	Fvector				direction;
	if (start_point)
		direction.sub	(position.position,position.center);
	else
		direction.sub	(position.point,position.center);

	float				yaw,pitch;
	direction.getHP		(yaw,pitch);
	for (u32 i=0, n=fis_zero(position.angle) ? 1 : iFloor(position.angular_velocity/(_abs(position.angle)*10.f) +.5f); i<n; ++i) {
		path.push_back	(Fvector());
		adjust_point	(position.center,yaw + float(i)*position.angle/float(n),position.radius,path.back());
	}
	return				(true);
}

bool build_line_trajectory(
	const STrajectoryPoint	&start, 
	const STrajectoryPoint	&dest, 
	xr_vector<Fvector>		&path
)
{
	xr_vector<u32>		node_path;
	u32					start_vertex_id = ai().level_graph().check_position_in_direction(start.start_vertex_id,start.position,start.point);
	if (!ai().level_graph().valid_vertex_id(start_vertex_id))
		return			(false);
	return				(ai().level_graph().create_straight_PTN_path(start_vertex_id,start.point,dest.point,path,node_path,false));
}

bool build_trajectory(
	const STrajectoryPoint	&start, 
	const STrajectoryPoint	&dest, 
	xr_vector<Fvector>		&path
)
{
	if (!build_circle_trajectory(start,path,true))
		return			(false);
	if (!build_line_trajectory(start,dest,path))
		return			(false);
	if (!build_circle_trajectory(dest,path,false))
		return			(false);
	return				(true);
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
	
	u32				tangent_count = 0;
	SCirclePoint	tangent_points[4][2];
	for (u32 i=0; i<2; ++i)
		for (u32 j=0; j<2; ++j)
			if (compute_tangent(
					start,
					start_circles[i],
					dest,
					dest_circles[j],
					tangent_points[tangent_count]
				)
			)
			++tangent_count;

	if (!choose_tangent(start,dest,tangent_points,tangent_count))
		return		(false);

	return			(build_trajectory(start,dest,path));
}