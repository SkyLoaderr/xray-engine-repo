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

template<
	typename T
>
IC	T sin_apb(T sina, T cosa, T sinb, T cosb)
{
	return				(sina*cosb + cosa*sinb);
}

template<
	typename T
>
IC	T cos_apb(T sina, T cosa, T sinb, T cosb)
{
	return				(cosa*cosb - sina*sinb);
}

bool CDetailPathManager::compute_tangent(
	const STrajectoryPoint	&start, 
	const SCirclePoint		&start_circle, 
	const STrajectoryPoint	&dest, 
	const SCirclePoint		&dest_circle, 
	SCirclePoint			*tangents
)
{
	float				start_cp, dest_cp, distance, alpha, start_yaw, dest_yaw, yaw1, yaw2;
	Fvector2			direction, temp;
	
	// computing 2D cross product for start point
	direction.sub		(start.position,start_circle.center);
	if (fis_zero(direction.square_magnitude()))
		direction		= start.direction;

	start_yaw			= direction.getH();
	start_yaw			= start_yaw >= 0.f ? start_yaw : start_yaw + PI_MUL_2;
	start_cp			= start.direction.cross_product(direction);
	
	// computing 2D cross product for dest point
	direction.sub		(dest.position,dest_circle.center);
	if (fis_zero(direction.square_magnitude()))
		direction		= dest.direction;
	dest_yaw			= direction.getH();
	dest_yaw			= dest_yaw >= 0.f ? dest_yaw : dest_yaw + PI_MUL_2;
	dest_cp				= dest.direction.cross_product(direction);

	// direction from the first circle to the second one
	direction.sub		(dest_circle.center,start_circle.center);
	yaw1				= direction.getH();
	yaw1 = yaw2			= yaw1 >= 0.f ? yaw1 : yaw1 + PI_MUL_2;

	if (start_cp*dest_cp >= 0.f) {
		// so, our tangents are outside
		if (start_circle.center.similar(dest_circle.center)) {
			if  (fsimilar(start_circle.radius,dest_circle.radius)) {
				// so, our circles are equal
				tangents[0]			= tangents[1] = start_circle;
				if (start_cp >= 0.f) {
					adjust_point	(start_circle.center,yaw1 + PI_DIV_2,	start_circle.radius,tangents[0].point);
					assign_angle	(tangents[0].angle,start_yaw,yaw1 + PI_DIV_2 < PI_MUL_2 ? yaw1 + PI_DIV_2 : yaw1 - PI - PI_DIV_2,true);
				}
				else {
					adjust_point	(start_circle.center,yaw1 - PI_DIV_2,	start_circle.radius,tangents[0].point);
					assign_angle	(tangents[0].angle,start_yaw,yaw1 - PI_DIV_2 >= 0.f ? yaw1 - PI_DIV_2 : yaw1 + PI + PI_DIV_2,false);
				}

				tangents[1].point	= tangents[0].point;
				tangents[1].angle	= 0.f;
				return				(true);
			}
			else
				return				(false);
		}
		else {
			// distance between circle centers
			distance		= start_circle.center.distance_to(dest_circle.center);
			// radius difference
			float			r_diff = start_circle.radius - dest_circle.radius;
			if ((r_diff > distance) && !fsimilar(r_diff,distance))
				return		(false);
			// angle between external tangents and circle centers segment
			float			temp = r_diff/distance;
			clamp			(temp,-.99999f,.99999f);
			alpha			= acosf(temp);
			alpha			= alpha >= 0.f ? alpha : alpha + PI_MUL_2;
		}
	}
	else {
		distance		= start_circle.center.distance_to(dest_circle.center);
		// so, our tangents are inside (crossing)
		if ((start_circle.radius + dest_circle.radius > distance) && !fsimilar(start_circle.radius + dest_circle.radius,distance))
			return		(false);
	
		// angle between internal tangents and circle centers segment
		float			temp = (start_circle.radius + dest_circle.radius)/distance;
		clamp			(temp,-.99999f,.99999f);
		alpha			= acosf(temp);
		alpha			= alpha >= 0.f ? alpha : alpha + PI_MUL_2;
		yaw2			= yaw1 < PI ? yaw1 + PI : yaw1 - PI;
	}

	tangents[0]			= start_circle;
	tangents[1]			= dest_circle;

	// compute external tangent points
	adjust_point		(start_circle.center,yaw1 + alpha,	start_circle.radius,tangents[0].point);
	adjust_point		(dest_circle.center, yaw2 + alpha,	dest_circle.radius, tangents[1].point);

	direction.sub		(tangents[1].point,tangents[0].point);
	temp.sub			(tangents[0].point,start_circle.center);
	float				tangent_cp = direction.cross_product(temp);
	if (start_cp*tangent_cp >= 0) {
		assign_angle	(tangents[0].angle,start_yaw,yaw1 + alpha < PI_MUL_2 ? yaw1 + alpha : yaw1 + alpha - PI_MUL_2,start_cp >= 0);
		assign_angle	(tangents[1].angle,dest_yaw, yaw2 + alpha < PI_MUL_2 ? yaw2 + alpha : yaw2 + alpha - PI_MUL_2,dest_cp  >= 0,false);
		return			(true);
	}

	// compute external tangent points
	adjust_point		(start_circle.center,yaw1 - alpha,	start_circle.radius,tangents[0].point);
	adjust_point		(dest_circle.center, yaw2 - alpha,	dest_circle.radius, tangents[1].point);
	assign_angle		(tangents[0].angle,start_yaw,yaw1 - alpha >= 0.f ? yaw1 - alpha : yaw1 - alpha + PI_MUL_2,start_cp >= 0);
	assign_angle		(tangents[1].angle,dest_yaw, yaw2 - alpha >= 0.f ? yaw2 - alpha : yaw2 - alpha + PI_MUL_2,dest_cp  >= 0,false);

	return				(true);
}

bool CDetailPathManager::build_circle_trajectory(
	const STrajectoryPoint		&position, 
	xr_vector<STravelPathPoint>	*path,
	u32							*vertex_id,
	const u32					velocity
)
{
	const float			min_dist = .1f;
	STravelPathPoint	t;
	t.velocity			= velocity;
	if (position.radius*_abs(position.angle) <= min_dist) {
		if (!path) {
			if (vertex_id)
				*vertex_id	= position.vertex_id;
			return			(true);
		}
		if (vertex_id) {
			*vertex_id		= position.vertex_id;
			if (path) {
				t.position			= ai().level_graph().v3d(position.position);
				t.position.y		= ai().level_graph().vertex_plane_y(ai().level_graph().vertex(position.vertex_id),position.position.x,position.position.y);
				path->push_back		(t);
			}
		}
		return			(true);
	}
	Fvector2			direction;
	Fvector				curr_pos;
	u32					curr_vertex_id;
	direction.sub		(position.position,position.center);
	curr_pos.set		(position.position.x,0.f,position.position.y);
	curr_pos.y			= ai().level_graph().vertex_plane_y(position.vertex_id,position.position.x,position.position.y);
	curr_vertex_id		= position.vertex_id;
	float				angle = position.angle;
	u32					size = path ? path->size() : u32(-1);

	if (!fis_zero(direction.square_magnitude()))
		direction.normalize	();
	else
		direction.set	(1.f,0.f);

	float				sina, cosa, sinb, cosb, sini, cosi, temp;
	u32					m = _min(iFloor(_abs(angle)/position.angular_velocity*10.f + 1.5f),iFloor(position.radius*_abs(angle)/min_dist + 1.5f));
	u32					n = fis_zero(position.angular_velocity) || !m ? 1 : m;
	int					k = vertex_id ? 0 : -1;

	if (path)
		path->reserve	(size + n + k);

	sina				= -direction.x;
	cosa				= direction.y;
	sinb				= _sin(angle/float(n));
	cosb				= _cos(angle/float(n));
	sini				= 0.f;
	cosi				= 1.f;

	for (u32 i=0; i<=n + k; ++i) {
		t.position.x	= -sin_apb(sina,cosa,sini,cosi)*position.radius + position.center.x;
		t.position.z	= cos_apb(sina,cosa,sini,cosi)*position.radius + position.center.y;
		
		curr_vertex_id	= ai().level_graph().check_position_in_direction(curr_vertex_id,curr_pos,t.position);
		if (!ai().level_graph().valid_vertex_id(curr_vertex_id))
			return		(false);

		if (path) {
			t.position.y	= ai().level_graph().vertex_plane_y(curr_vertex_id,t.position.x,t.position.z);
			path->push_back	(t);
		}

		temp			= sin_apb(sinb,cosb,sini,cosi);
		cosi			= cos_apb(sinb,cosb,sini,cosi);
		sini			= temp;
		curr_pos		= t.position;
	}
	
	if (vertex_id)
		*vertex_id		= curr_vertex_id;
	else
		if (path)
			reverse		(path->begin() + size,path->end());

	return				(true);
}

bool CDetailPathManager::build_line_trajectory(
	const STrajectoryPoint		&start, 
	const STrajectoryPoint		&dest, 
	u32							vertex_id,
	xr_vector<STravelPathPoint>	*path,
	u32							velocity
)
{
	VERIFY					(ai().level_graph().valid_vertex_id(vertex_id));
	STravelPathPoint		t;
	t.velocity				= velocity;
	if (ai().level_graph().inside(vertex_id,dest.point)) {
		if (path) {
			t.position		= ai().level_graph().v3d(dest.point);
			t.position.y	= ai().level_graph().vertex_plane_y(vertex_id,dest.point.x,dest.point.y);
			path->push_back	(t);
		}
		return				(true);
	}
	return					(path ? ai().level_graph().create_straight_PTN_path(vertex_id,start.point,dest.point,*path,t,false,false) : ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(vertex_id,start.point,dest.point)));
}

bool CDetailPathManager::build_trajectory(
	const STrajectoryPoint		&start, 
	const STrajectoryPoint		&dest, 
	xr_vector<STravelPathPoint>	*path,
	const u32					velocity1,
	const u32					velocity2,
	const u32					velocity3
)
{
	u32					vertex_id;
	if (!build_circle_trajectory(start,path,&vertex_id,velocity1))
		return			(false);
	if (!build_line_trajectory(start,dest,vertex_id,path,velocity2))
		return			(false);
	if (!build_circle_trajectory(dest,path,0,velocity3))
		return			(false);
	return				(true);
}

bool CDetailPathManager::build_trajectory(
	STrajectoryPoint			&start, 
	STrajectoryPoint			&dest, 
	const SCirclePoint			tangents[4][2], 
	const u32					tangent_count,
	xr_vector<STravelPathPoint>	*path,
	float						&time,
	const u32					velocity1,
	const u32					velocity2,
	const u32					velocity3
)
{
	time			= flt_max;
	SDist			dist[4];
	xr_map<u32,STravelParams>::const_iterator I = m_movement_params.find(velocity2);
	VERIFY			(m_movement_params.end() != I);
	float			straight_velocity = (*I).second.linear_velocity;
	{
		for (u32 i=0; i<tangent_count; ++i) {
			dist[i].index = i;
			dist[i].time = 
				_abs(tangents[i][0].angle)/start.angular_velocity +
				_abs(tangents[i][1].angle)/dest.angular_velocity +
				tangents[i][0].point.distance_to(tangents[i][1].point)*
				(fis_zero(straight_velocity) ? 0 : 1.f/straight_velocity); 
		}
	}
	
	std::sort	(dist,dist + tangent_count);

	{
		for (u32 i=0, j = path ? path->size() : 0; i<tangent_count; ++i) {
			(SCirclePoint&)(start) = tangents[dist[i].index][0];
			(SCirclePoint&)(dest)	= tangents[dist[i].index][1];
			if (build_trajectory(start,dest,path,velocity1,velocity2,velocity3)) {
				time	= dist[i].time;
				return	(true);
			}
			else
				if (path)
					path->resize(j);
		}
	}

	return		(false);
}

bool CDetailPathManager::compute_trajectory(
	STrajectoryPoint			&start,
	STrajectoryPoint			&dest,
	xr_vector<STravelPathPoint>	*path,
	float						&time,
	const u32					velocity1,
	const u32					velocity2,
	const u32					velocity3
)
{
	SCirclePoint				start_circles[2], dest_circles[2];
	compute_circles				(start,start_circles);
	compute_circles				(dest,dest_circles);
	
	u32							tangent_count = 0;
	SCirclePoint				tangent_points[4][2];
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

	return			(build_trajectory(start,dest,tangent_points,tangent_count,path,time,velocity1,velocity2,velocity3));
}

bool CDetailPathManager::compute_path(
	STrajectoryPoint			&start,
	STrajectoryPoint			&dest,
	xr_vector<STravelPathPoint>	*m_tpTravelLine
)
{
	m_temp_path.clear		();
	float					min_time = flt_max, time;
	u32						size = m_tpTravelLine ? m_tpTravelLine->size() : 0;
	xr_map<u32,STravelParams>::const_iterator I = m_movement_params.begin(), B = I;
	xr_map<u32,STravelParams>::const_iterator E = m_movement_params.end();
	for ( ; I != E; ++I) {
		(STravelParams&)start	= (*I).second;
		xr_map<u32,STravelParams>::const_iterator i = m_movement_params.begin(), b = i;
		xr_map<u32,STravelParams>::const_iterator e = m_movement_params.end();
		for ( ; i != e; ++i) {
			(STravelParams&)dest	= (*i).second;
			m_temp_path.clear		();
			if (compute_trajectory(start,dest,m_tpTravelLine ? &m_temp_path : 0,time,(*I).first,0,(*i).first)) {
				if (time < min_time) {
					min_time = time;
					if (m_tpTravelLine) {
						m_tpTravelLine->resize(size);
						m_tpTravelLine->insert(m_tpTravelLine->end(),m_temp_path.begin(),m_temp_path.end());
					}
					else
						return(true);
				}
			}
		}
	}
	
	if (fsimilar(min_time,flt_max))
		return				(false);
	
	return					(true);
}

void CDetailPathManager::build_smooth_path		(
	const xr_vector<u32>	&level_path, 
	u32						intermediate_index
)
{
	build_straight_path(level_path,intermediate_index);
	m_current_travel_point					= 0;
//	Device.Statistic.AI_Range.Begin			();
//
//	STrajectoryPoint						s,d,t,p,start,dest;
//
//	start.position							= ai().level_graph().v2d(m_start_position);
//	start.direction							= ai().level_graph().v2d(m_start_direction);
//	
//	dest.position							= ai().level_graph().v2d(m_dest_position);
//	dest.direction							= ai().level_graph().v2d(m_dest_direction);
//
//	VERIFY									(!level_path.empty());
//	VERIFY									(level_path.size() > intermediate_index);
//
//	start.direction.normalize				();
//	dest.direction.normalize				();
//	m_path.clear							();
//
//	if (m_tpaNodes.size() == 1) {
//		if (!compute_path(start,dest,&m_path)) {
//			m_path.clear					();
//			Device.Statistic.AI_Range.End	();
//			return;
//		}
//	}
//	else {
//		if (compute_path(start,dest,&m_path)) {
//			Device.Statistic.AI_Range.End	();
//			return;
//		}
//		
//		m_path.clear						();
//
//		STravelPoint						start_point;
//		start_point.vertex_id				= m_tpaNodes.front();
//		start_point.position				= start.position;
//
//		for (int _i=0, i=0, n=(int)m_tpaNodes.size() - 1, j = n, m=j; _i < n; ) {
//			if (!ai().level_graph().check_vertex_in_direction(start_point.vertex_id,start_point.position,m_tpaNodes[j])) {
//				m							= j;
//				j							= (i + j)/2;
//			}
//			else {
//				i							= j;
//				j							= (i + m)/2;
//			}
//			if (i >= m - 1) {
//				if (i <= _i) {
//					Device.Statistic.AI_Range.End();
//					return;
//				}
//				_i							= i;
//				m_key_points.push_back		(start_point);
//				if (i == n) {
//					if (ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(start_point.vertex_id,start_point.position,dest.position))) {
//						m_key_points.push_back(dest);
//						break;
//					}
//					else {
//						Device.Statistic.AI_Range.End();
//						return;
//					}
//				}
//				start_point.vertex_id		= m_tpaNodes[_i];
//				start_point.position		= ai().level_graph().v2d(ai().level_graph().vertex_position(start_point.vertex_id));
//				j = m						= n;
//			}
//		}
//
//		s = t								= start;
//		xr_vector<STravelPoint>::const_iterator	I = m_key_points.begin(), P = I;
//		xr_vector<STravelPoint>::const_iterator	E = m_key_points.end();
//		for ( ; I != E; ++I) {
//			// setting up destination
//			if ((I + 1) != E) {
//				(STravelPoint&)d = *I;
//				d.direction.sub				((I + 1)->position,d.position);
//				VERIFY						(!fis_zero(d.direction.magnitude()));
//				d.direction.normalize		();
//			}
//			else
//				d							= dest;
//
//			if (!compute_path(s,d,0)) {
//				if (I == P) {
//					m_path.clear			();
//					Device.Statistic.AI_Range.End();
//					return;
//				}
//				
//				p							= d;
//				(STravelPoint&)d = *(I - 1);
//				d.direction.sub				((*I).position,d.position);
//				VERIFY						(!fis_zero(d.direction.magnitude()));
//				d.direction.normalize		();
//				if (!compute_path(s,d,&m_path)) {
//					VERIFY					(false);
//				}
//				P							= I - 1;
//				d							= p;
//				s							= t;
//				VERIFY						(!fis_zero(s.direction.magnitude()));
//				if (!compute_path(s,d,0)) {
//					m_path.clear			();
//					Device.Statistic.AI_Range.End();
//					return;
//				}
//			}
//			t								= d;
//		}
//		if (!compute_path(s,d,&m_path)) {
//			if (compute_path(s,d,0)) {
//				compute_path(s,d,&m_path);
//				t=t;
//			}
//			VERIFY							(false);
//		}
//	}
//	m_current_travel_point					= 0;
//
//	Device.Statistic.AI_Range.End			();
}