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

#define SMOOTH_PATH

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

bool is_negative(float a)
{
	return				(!fis_zero(a) && (a < 0.f));
}

bool CDetailPathManager::compute_tangent(
	const STrajectoryPoint	&start, 
	const SCirclePoint		&start_circle, 
	const STrajectoryPoint	&dest, 
	const SCirclePoint		&dest_circle, 
	SCirclePoint			*tangents,
	const EDirectionType	direction_type
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
					assign_angle	(tangents[0].angle,start_yaw,yaw1 + PI_DIV_2 < PI_MUL_2 ? yaw1 + PI_DIV_2 : yaw1 - PI - PI_DIV_2,true,direction_type);
				}
				else {
					adjust_point	(start_circle.center,yaw1 - PI_DIV_2,	start_circle.radius,tangents[0].point);
					assign_angle	(tangents[0].angle,start_yaw,yaw1 - PI_DIV_2 >= 0.f ? yaw1 - PI_DIV_2 : yaw1 + PI + PI_DIV_2,false,direction_type);
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
		assign_angle	(tangents[0].angle,start_yaw,yaw1 + alpha < PI_MUL_2 ? yaw1 + alpha : yaw1 + alpha - PI_MUL_2,start_cp >= 0,direction_type);
		assign_angle	(tangents[1].angle,dest_yaw, yaw2 + alpha < PI_MUL_2 ? yaw2 + alpha : yaw2 + alpha - PI_MUL_2,dest_cp  >= 0,direction_type,false);
		return			(true);
	}

	// compute external tangent points
	adjust_point		(start_circle.center,yaw1 - alpha,	start_circle.radius,tangents[0].point);
	adjust_point		(dest_circle.center, yaw2 - alpha,	dest_circle.radius, tangents[1].point);
	assign_angle		(tangents[0].angle,start_yaw,yaw1 - alpha >= 0.f ? yaw1 - alpha : yaw1 - alpha + PI_MUL_2,start_cp >= 0,direction_type);
	assign_angle		(tangents[1].angle,dest_yaw, yaw2 - alpha >= 0.f ? yaw2 - alpha : yaw2 - alpha + PI_MUL_2,dest_cp  >= 0,direction_type,false);

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
			t.position		= ai().level_graph().v3d(position.position);
			t.vertex_id		= position.vertex_id;
			path->push_back	(t);

		}
		return			(true);
	}
	Fvector2			direction;
	Fvector				curr_pos;
	u32					curr_vertex_id;
	direction.sub		(position.position,position.center);
	curr_pos.set		(position.position.x,0.f,position.position.y);
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
			t.vertex_id		= curr_vertex_id;
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
	VERIFY				(ai().level_graph().valid_vertex_id(vertex_id));
	STravelPathPoint	t;
	t.velocity			= velocity;
	if (ai().level_graph().inside(vertex_id,dest.point)) {
		if (path) {
			t.position	= ai().level_graph().v3d(dest.point);
			t.vertex_id	= vertex_id;
			path->push_back(t);
		}
		return			(true);
	}
	return				(path ? ai().level_graph().create_straight_PTN_path<false>(vertex_id,start.point,dest.point,*path,t,false,false) : ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(vertex_id,start.point,dest.point)));
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
	float			straight_velocity = _abs((*I).second.linear_velocity);
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
	const u32					velocity3,
	const EDirectionType		direction_type
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
					tangent_points[tangent_count],
					direction_type
				)
			)
				++tangent_count;

	return			(build_trajectory(start,dest,tangent_points,tangent_count,path,time,velocity1,velocity2,velocity3));
}

bool CDetailPathManager::compute_path(
	STrajectoryPoint			&_start,
	STrajectoryPoint			&_dest,
	xr_vector<STravelPathPoint>	*m_tpTravelLine,
	const xr_vector<STravelParamsIndex> &start_params,
	const xr_vector<STravelParamsIndex> &dest_params,
	const u32					straight_line_index,
	const u32					straight_line_index_negative
)
{
	STrajectoryPoint			start = _start;
	STrajectoryPoint			dest = _dest;
	float						min_time = flt_max, time;
	u32							size = m_tpTravelLine ? m_tpTravelLine->size() : 0;
	u32							real_straight_line_index;
	xr_vector<STravelParamsIndex>::const_iterator I = start_params.begin(), B = I;
	xr_vector<STravelParamsIndex>::const_iterator E = start_params.end();
	for ( ; I != E; ++I) {
		EDirectionType				direction_type = eDirectionTypePP;
		start						= _start;
		(STravelParams&)start		= (*I);
		real_straight_line_index	= straight_line_index;
		if (is_negative(start.linear_velocity)) {
			real_straight_line_index= straight_line_index_negative;
			direction_type			= EDirectionType(direction_type | eDirectionTypeFN);
			start.direction.mul		(-1.f);
		}
		xr_vector<STravelParamsIndex>::const_iterator i = dest_params.begin(), b = i;
		xr_vector<STravelParamsIndex>::const_iterator e = dest_params.end();
		for ( ; i != e; ++i) {
			dest					= _dest;
			(STravelParams&)dest	= (*i);

			if (is_negative(dest.linear_velocity))
				direction_type		= EDirectionType(direction_type | eDirectionTypeSN);

			if (direction_type & eDirectionTypeFN)
				dest.direction.mul	(-1.f);

			m_temp_path.clear		();
			if (compute_trajectory(start,dest,m_tpTravelLine ? &m_temp_path : 0,time,(*I).index,real_straight_line_index,(*i).index,direction_type)) {
				if (!m_try_min_time || (time < min_time)) {
					min_time		= time;
					if (m_tpTravelLine) {
						m_tpTravelLine->resize(size);
						m_tpTravelLine->insert(m_tpTravelLine->end(),m_temp_path.begin(),m_temp_path.end());

						if (!m_try_min_time)
							return	(true);
					}
					else
						return		(true);
				}
			}
		}
	}
	
	if (fsimilar(min_time,flt_max))
		return				(false);
	
	return					(true);
}

void CDetailPathManager::validate_vertex_position(STrajectoryPoint &point) const
{
	if (ai().level_graph().valid_vertex_position(ai().level_graph().v3d(point.position)) && ai().level_graph().inside(point.vertex_id,point.position))
		return;

	CLevelGraph::SContour	contour;
	Fvector					position, center;
	ai().level_graph().contour(contour,point.vertex_id);
	ai().level_graph().nearest(position,ai().level_graph().v3d(point.position),contour);
	center.add				(contour.v1,contour.v3);
	center.mul				(.5f);
	center.sub				(position);
	center.normalize		();
	center.mul				(EPS_L);
	position.add			(center);
	point.position			= ai().level_graph().v2d(position);
	VERIFY					(ai().level_graph().inside(point.vertex_id,point.position));
}

bool CDetailPathManager::init_build(
	const xr_vector<u32>	&level_path, 
	u32						intermediate_index,
	STrajectoryPoint		&start,
	STrajectoryPoint		&dest,
	u32						&straight_line_index,
	u32						&straight_line_index_negative
)
{
	VERIFY								(!level_path.empty());
	VERIFY								(level_path.size() > intermediate_index);

	m_current_travel_point				= 0;
	m_path.clear						();

	start.position						= ai().level_graph().v2d(m_start_position);
	start.direction						= ai().level_graph().v2d(m_start_direction);
	start.vertex_id						= level_path.front();
	
	validate_vertex_position			(start);
	
	dest.position						= ai().level_graph().v2d(m_dest_position);
	if (m_use_dest_orientation)
		dest.direction					= ai().level_graph().v2d(m_dest_direction);
	else
		dest.direction.set				(0.f,1.f);
	dest.vertex_id						= level_path.back();
	
	validate_vertex_position			(dest);

	if (start.direction.square_magnitude() < EPS_L)
		start.direction.set				(0.f,1.f);
	else
		start.direction.normalize		();

	if (dest.direction.square_magnitude() < EPS_L)
		dest.direction.set				(0.f,1.f);
	else
		dest.direction.normalize		();

	// filling velocity parameters
	float								max_linear_velocity = -flt_max;
	float								min_linear_velocity = flt_max;
	straight_line_index					= u32(-1);
	straight_line_index_negative		= u32(-1);
	m_start_params.clear				();
	xr_map<u32,STravelParams>::const_iterator I = m_movement_params.begin(), B = I;
	xr_map<u32,STravelParams>::const_iterator E = m_movement_params.end();
	for ( ; I != E; ++I) {
		if (!check_mask(m_velocity_mask,(*I).first))
			continue;
		STravelParamsIndex				temp;
		(STravelParams&)temp			= (*I).second;
		temp.index						= (*I).first;
		if (check_mask(m_desirable_mask,(*I).first)) {
			m_start_params.insert		(m_start_params.begin(),temp);
			if (max_linear_velocity < temp.linear_velocity) {
				straight_line_index		= temp.index;
				max_linear_velocity		= temp.linear_velocity;
			}
			if (min_linear_velocity > temp.linear_velocity) {
				straight_line_index_negative = temp.index;
				min_linear_velocity		= temp.linear_velocity;
			}
		}
		else
			m_start_params.push_back	(temp);
	}

	if (m_start_params.empty())
		return							(false);

	m_dest_params.clear					();
	m_dest_params.push_back				(STravelParamsIndex(0.f,PI_MUL_2,u32(-1)));

	return								(true);
}

bool CDetailPathManager::fill_key_points(
	const xr_vector<u32>	&level_path, 
	u32						intermediate_index,
	STrajectoryPoint		&start,
	STrajectoryPoint		&dest
)
{
	STravelPoint						start_point;
	start_point.vertex_id				= start.vertex_id;
	start_point.position				= start.position;
	m_key_points.clear					();

	for (int _i=0, i=0, n=(int)level_path.size() - 1, j = n, m=j; _i < n; ) {
		if (!ai().level_graph().check_vertex_in_direction(start_point.vertex_id,start_point.position,level_path[j])) {
			m							= j;
			j							= (i + j)/2;
		}
		else {
			if ((j == n) && !ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(start_point.vertex_id,start_point.position,dest.position))) {
				m						= j;
				j						= (i + j)/2;
			}
			else {
				i						= j;
				j						= (i + m)/2;
			}
		}
		if (i >= m - 1) {
			if (i <= _i)
				return					(false);
			_i							= i;
			m_key_points.push_back		(start_point);
			if (i == n) {
				if (ai().level_graph().valid_vertex_id(ai().level_graph().check_position_in_direction(start_point.vertex_id,start_point.position,dest.position))) {
					m_key_points.push_back	(dest);
					break;
				}
				else
					return				(false);
			}
			start_point.vertex_id		= level_path[_i];
			start_point.position		= ai().level_graph().v2d(ai().level_graph().vertex_position(start_point.vertex_id));
			j = m						= n;
		}
	}
	return								(true);
}

void CDetailPathManager::build_path_via_key_points(
	STrajectoryPoint		&start,
	STrajectoryPoint		&dest,
	xr_vector<STravelParamsIndex> &finish_params,
	const u32				straight_line_index,
	const u32				straight_line_index_negative
)
{
	STrajectoryPoint					s,d,p;
	s 									= start;
	xr_vector<STravelPoint>::const_iterator	I = m_key_points.begin(), P = I;
	xr_vector<STravelPoint>::const_iterator	E = m_key_points.end();
	for ( ; I != E; ++I) {
		// setting up destination
		if (!ai().level_graph().inside((*I).vertex_id,(*I).position)) {
			VERIFY					(false);
		}
		if ((I + 1) != E) {
			(STravelPoint&)d = *I;
			d.direction.sub				((I + 1)->position,d.position);
			VERIFY						(!fis_zero(d.direction.magnitude()));
			d.direction.normalize		();
		}
		else
			d							= dest;

		if (!compute_path(s,d,0,m_start_params,m_dest_params,straight_line_index,straight_line_index_negative)) {
			if (I == P) {
				m_path.clear			();
				return;
			}

			p							= d;
			(STravelPoint&)d			= *(I - 1);
			d.direction.sub				((*I).position,d.position);
			VERIFY						(!fis_zero(d.direction.magnitude()));
			d.direction.normalize		();
			if (!compute_path(s,d,&m_path,m_start_params,m_dest_params,straight_line_index,straight_line_index_negative)) {
				compute_path			(s,d,0,m_start_params,m_dest_params,straight_line_index,straight_line_index_negative);
				compute_path			(s,d,&m_path,m_start_params,m_dest_params,straight_line_index,straight_line_index_negative);
				m_path.clear			();
				return;
				//VERIFY				(false);
			}
			P							= I - 1;

			s							= d;

			VERIFY						(m_path.size() > 1);
			s.direction.sub				(
				ai().level_graph().v2d(
					m_path[m_path.size()-1].position
				),
				ai().level_graph().v2d(
					m_path[m_path.size()-2].position
				)
			);
			VERIFY						(s.direction.square_magnitude() > EPS_L);
			s.direction.normalize		();
			m_path.pop_back				();

			d							= p;
			if (!m_path.empty()) {
				xr_map<u32,STravelParams>::const_iterator I = m_movement_params.find(m_path.back().velocity);
				VERIFY					(m_movement_params.end() != I);
				if (is_negative((*I).second.linear_velocity))
					s.direction.mul		(-1.f);
			}

			VERIFY						(!fis_zero(s.direction.magnitude()));
			if (!compute_path(s,d,0,m_start_params,m_dest_params,straight_line_index,straight_line_index_negative)) {
				m_path.clear			();
				return;
			}
		}
	}

	if (!compute_path(s,d,&m_path,m_start_params,finish_params,straight_line_index,straight_line_index_negative)) {
//		compute_path(s,d,0,m_start_params,finish_params,straight_line_index,straight_line_index_negative);
//		compute_path(s,d,&m_path,m_start_params,finish_params,straight_line_index,straight_line_index_negative);
		m_path.clear					();
		return;
//		VERIFY							(false);
	}

	add_patrol_point					();
	ai().level_graph().assign_y_values	(m_path);
	m_failed							= false;
}

void CDetailPathManager::add_patrol_point()
{
	m_last_patrol_point					= m_path.size() - 1;
	if ((m_path.size() > 1) && m_state_patrol_path) {
		STravelPathPoint				t;
		Fvector							v;
		v.sub							(m_path.back().position,m_path[m_last_patrol_point - 1].position);
		v.y								= 0.f;
		if (v.magnitude() > EPS_S)
			v.normalize					();
		else
			return;

		v.mul							(4.f);
		t								= m_path.back();
		t.position.add					(v);
		ai().level_graph().create_straight_PTN_path<false>(
			m_path.back().vertex_id,
			ai().level_graph().v2d(m_path.back().position),
			ai().level_graph().v2d(t.position),
			m_path,
			m_path.back(),
			false,
			false
		);
	}
}

void CDetailPathManager::build_smooth_path		(
	const xr_vector<u32>	&level_path, 
	u32						intermediate_index
)
{
	Device.Statistic.AI_Range.Begin		();
#ifndef SMOOTH_PATH
	build_straight_path					(level_path,intermediate_index);
	m_current_travel_point				= 0;
	m_last_patrol_point					= m_path.size() - 1;
#else
	m_failed							= true;
	u32									straight_line_index, straight_line_index_negative;

	STrajectoryPoint					start,dest;

	if (!init_build(level_path,intermediate_index,start,dest,straight_line_index,straight_line_index_negative)) {
		Device.Statistic.AI_Range.End	();
		return;
	}

	xr_vector<STravelParamsIndex>		&finish_params = m_use_dest_orientation ? m_start_params : m_dest_params;

	if (compute_path(start,dest,&m_path,m_start_params,finish_params,straight_line_index,straight_line_index_negative)) {
		add_patrol_point();
		ai().level_graph().assign_y_values(m_path);
		m_failed						= false;
		Device.Statistic.AI_Range.End	();
		return;
	}
	
	if (level_path.size() == 1) {
		m_path.clear					();
		Device.Statistic.AI_Range.End	();
		return;
	}

	if (!fill_key_points(level_path,intermediate_index,start,dest)) {
		Device.Statistic.AI_Range.End	();
		return;
	}
	
	build_path_via_key_points			(start,dest,finish_params,straight_line_index,straight_line_index_negative);
#endif
	Device.Statistic.AI_Range.End		();
}