#include "stdafx.h"
#include "HelicopterMovementManager.h"

bool is_negative_(float a)
{
	return				(!fis_zero(a) && (a < 0.f));
}

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

bool 
CHelicopterMovementManager::compute_path(
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
		if (is_negative_(start.linear_velocity)) 
		{
			real_straight_line_index= straight_line_index_negative;
			direction_type			= EDirectionType(direction_type | eDirectionTypeFN);
			start.direction.mul		(-1.f);
		}
		xr_vector<STravelParamsIndex>::const_iterator i = dest_params.begin(), b = i;
		xr_vector<STravelParamsIndex>::const_iterator e = dest_params.end();
		for ( ; i != e; ++i) {
			dest					= _dest;
			(STravelParams&)dest	= (*i);

			if (is_negative_(dest.linear_velocity))
				direction_type		= EDirectionType(direction_type | eDirectionTypeSN);

			if (direction_type & eDirectionTypeFN)
				dest.direction.mul	(-1.f);

			m_temp_path.clear		();
			if (compute_trajectory(	start,
									dest,
									m_tpTravelLine ? &m_temp_path : 0,
									time,
									(*I).index,
									real_straight_line_index,
									(*i).index,
									direction_type)) 
			{
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

bool 
CHelicopterMovementManager::compute_trajectory(
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

	return			
		(build_trajectory(start,dest,tangent_points,tangent_count,path,time,velocity1,velocity2,velocity3));
}

bool 
CHelicopterMovementManager::compute_tangent(
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


bool //second
CHelicopterMovementManager::build_trajectory(
	const STrajectoryPoint		&start, 
	const STrajectoryPoint		&dest, 
	xr_vector<STravelPathPoint>	*path,
	const u32					velocity1,
	const u32					velocity2,
	const u32					velocity3
	)
{
	STravelPathPoint lastAdded;
	if (!build_circle_trajectory(start, path, velocity1, lastAdded, true))
		return			(false);
	if (!build_line_trajectory(lastAdded, dest, path, velocity2, lastAdded))
		return			(false);
	if (!build_circle_trajectory(dest, path, velocity3, lastAdded, false))
		return			(false);
	return				(true);
}

bool //first
CHelicopterMovementManager::build_trajectory(
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


bool 
CHelicopterMovementManager::build_circle_trajectory(
	const STrajectoryPoint		&position, 
	xr_vector<STravelPathPoint>	*path,
	const u32					velocity,
	STravelPathPoint&			lastAddedPoint, 
	bool fromCenter
	)
{
	const float			min_dist = .1f;
	STravelPathPoint	t;
	t.velocity			= velocity;
	if (position.radius*_abs(position.angle) <= min_dist) 
	{
		t.position		= v3d(position.position);
		path->push_back(t);
		lastAddedPoint = t;
		return			(true);

	}
	Fvector2			direction;
	Fvector				curr_pos;
	direction.sub		(position.position,position.center);
	curr_pos.set		(position.position.x,0.f,position.position.y);
	float				angle = position.angle;
	u32					size = path ? path->size() : u32(-1);

	if (!fis_zero(direction.square_magnitude()))
		direction.normalize	();
	else
		direction.set	(1.f,0.f);

	float				sina, cosa, sinb, cosb, sini, cosi, temp;
	u32					m = _min(iFloor(_abs(angle)/position.angular_velocity*10.f + 1.5f),iFloor(position.radius*_abs(angle)/min_dist + 1.5f));
	u32					n = fis_zero(position.angular_velocity) || !m ? 1 : m;
	int					k = 0;

	if (path)
		path->reserve	(size + n);

	sina				= -direction.x;
	cosa				= direction.y;
	sinb				= _sin(angle/float(n));
	cosb				= _cos(angle/float(n));
	sini				= 0.f;
	cosi				= 1.f;

	for (u32 i=0; i<=n + k; ++i) {
		t.position.x	= -sin_apb(sina,cosa,sini,cosi)*position.radius + position.center.x;
		t.position.z	= cos_apb(sina,cosa,sini,cosi)*position.radius + position.center.y;
		t.position.y	= 0.0f;
		

		if (path) {
			path->push_back	(t);
			lastAddedPoint = t;
		}

		temp			= sin_apb(sinb,cosb,sini,cosi);
		cosi			= cos_apb(sinb,cosb,sini,cosi);
		sini			= temp;
		curr_pos		= t.position;
	}

	if(!fromCenter)
		reverse		(path->begin() + size,path->end());

	return				(true);
}

bool CHelicopterMovementManager::build_line_trajectory(
	const STravelPathPoint		&start, 
	const STrajectoryPoint		&_dest, 
//	u32							vertex_id,
	xr_vector<STravelPathPoint>	*path,
	u32							velocity,
	STravelPathPoint&			lastAddedPoint
	)
{
//	Fvector dest = v3d(_dest.position);
	Fvector dest = v3d(_dest.point);
	float dist = start.position.distance_to(dest);
	Fvector v;
	v.sub(dest, start.position);
	float m = v.magnitude();
	float d = dist*.5f;
	v.div(d);

	lastAddedPoint = start;
	for(float f=0.0f; (d-f)>1.9f; f+=1.0f)
	{
		lastAddedPoint.position.add(v);
		path->push_back(lastAddedPoint);
	}

	return true;
}

void 
CHelicopterMovementManager::validate_vertex_position(STrajectoryPoint &point) const
{
	/*
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
	*/
}

void 
CHelicopterMovementManager::build_smooth_path (int startKeyIdx, bool bClearOld)
{
	m_failed							= true;
	u32									straight_line_index, straight_line_index_negative;

	STrajectoryPoint					start,dest;
	u32									oldSize;
	if(bClearOld)
	{
		m_path.clear						();
	}else
		oldSize = m_path.size();

	if (!init_build(startKeyIdx,start,dest,straight_line_index,straight_line_index_negative)) 
		return;

	xr_vector<STravelParamsIndex>		&finish_params = m_use_dest_orientation ? m_start_params : m_dest_params;

	if (compute_path(start,dest,&m_path,m_start_params,finish_params,straight_line_index,straight_line_index_negative)) 
	{
		//		add_patrol_point();
		for(pathIt It = m_path.begin(); It!=m_path.end(); ++It)
			(*It).position.y = 2.0f;

		m_failed						= false;
		m_curPathIdx					= 0;

		//temporary solution -- calcutating time
		pathIt B  = m_path.begin();
		pathIt B_ = m_path.begin();
		u32 time = Level().timeServer();
		
		if(!bClearOld)
		{
			std::advance(B, oldSize-1);
			std::advance(B_, oldSize-1);
			time = (*B).time;
		}

		float velocity = m_movement_params[0].linear_velocity;

		(*B).time = time;

		++B;
		for(;B!=m_path.end();++B,++B_)
		{
			float dist = (*B_).position.distance_to( (*B).position );
			u32 t = (*B_).time + (dist/velocity)*1000;
			(*B).time = t;
		};
	}
}

bool 
CHelicopterMovementManager::init_build(int startKeyIdx, 
									   STrajectoryPoint		&start,
									   STrajectoryPoint		&dest,
									   u32					&straight_line_index,
									   u32					&straight_line_index_negative
									   )
{
	u32 idxP1,idxP2;
	idxP1 = startKeyIdx;

	m_use_dest_orientation = true;
	m_cycle_path = true;

	if( startKeyIdx >= m_keyTrajectory.size()-1 )
	{
		if(m_cycle_path)
		{
			idxP2 = 0;
			m_currKeyIdx = -1;
		}else
			return false;
	}else
		idxP2 = startKeyIdx+1;

	m_movement_params.insert(std::make_pair(0, STravelParams(33.0f,	PI)));
//	m_movement_params.insert(std::make_pair(1, STravelParams(10.0f, PI_MUL_2)));
//	m_movement_params.insert(std::make_pair(2, STravelParams(5.0f, PI_MUL_4)));

//	m_current_travel_point				= 0;

//	start.position						= v2d(m_start_position);
//	start.direction						= v2d(m_start_direction);
	start.position						= v2d(m_keyTrajectory[idxP1].position);
	start.direction						= v2d(m_keyTrajectory[idxP1].direction);
	

	validate_vertex_position			(start);

	dest.position						= v2d(m_keyTrajectory[idxP2].position);
	if (m_use_dest_orientation)
		dest.direction					= v2d(m_keyTrajectory[idxP2].direction);
	else
		dest.direction.set				(0.f,1.f);

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
	for ( ; I != E; ++I) 
	{
		//		if (!check_mask(m_velocity_mask,(*I).first))
		//			continue;
		STravelParamsIndex				temp;
		(STravelParams&)temp			= (*I).second;
		temp.index						= (*I).first;

		//		if (check_mask(m_desirable_mask,(*I).first)) 
		{
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
		//		else
		//			m_start_params.push_back	(temp);
	}

	if (m_start_params.empty())
		return							(false);

	m_dest_params.clear					();
	m_dest_params.push_back				(STravelParamsIndex(0.f,PI_MUL_2,u32(-1)));

	return								(true);
}