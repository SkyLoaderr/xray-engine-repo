#include "stdafx.h"
#include "level_graph.h"
#include "graph_engine.h"
#include "profile.h"

struct STravelParams {
	float			linear_velocity;
	float			angular_velocity; 
};

struct STravelPoint {
	Fvector			position;
	u32				vertex_id;
};

struct SPathPoint : public STravelParams, public STravelPoint {
	Fvector			direction;
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
	TIMER_START(AdjustPoint)
	dest.setHP			(yaw,0);
	dest.mul			(magnitude);
	dest.add			(source);
	TIMER_STOP(AdjustPoint)
}

IC	void assign_angle(
	float				&angle, 
	const float			start_yaw, 
	const float			dest_yaw, 
	const bool			positive,
	const bool			start = true
)
{
	TIMER_START(AssignAngle)
	if (positive)
		if (dest_yaw >= start_yaw)
			angle		= dest_yaw - start_yaw;
		else
			angle		= PI_MUL_2 - start_yaw + dest_yaw;
	else
		if (dest_yaw <= start_yaw)
			angle		= dest_yaw - start_yaw;
		else
			angle		= dest_yaw - start_yaw - PI_MUL_2;

	if (!start)
		if (angle < 0.f)
			angle = -angle - PI_MUL_2;
		else
			angle = PI_MUL_2 - angle;
	TIMER_STOP(AssignAngle)
}

IC	void compute_circles(
	STrajectoryPoint	&point, 
	SCirclePoint		*circles
)
{
	TIMER_START(ComputeCircles)
	VERIFY				(!fis_zero(point.angular_velocity));
	point.radius		= point.linear_velocity/point.angular_velocity;
	circles[0].radius	= circles[1].radius = point.radius;
	float				yaw, pitch;
	VERIFY				(!fis_zero(point.direction.square_magnitude()));
	point.direction.getHP(yaw,pitch);
	adjust_point		(point.position,yaw + PI_DIV_2,point.radius,circles[0].center);
	adjust_point		(point.position,yaw - PI_DIV_2,point.radius,circles[1].center);
	TIMER_STOP(ComputeCircles)
}

IC	float cross_product_2D_y(
	const Fvector &point1,
	const Fvector &point2
)
{
	TIMER_START(CrossProduct2D)
	float				f = point1.z*point2.x - point1.x*point2.z;
	TIMER_STOP(CrossProduct2D)
	return				(f);
}

IC	bool compute_tangent(
	const STrajectoryPoint	&start, 
	const SCirclePoint		&start_circle, 
	const STrajectoryPoint	&dest, 
	const SCirclePoint		&dest_circle, 
	SCirclePoint			*tangents
)
{
	TIMER_START(ComputeTangent)
	float				start_cp, dest_cp, distance, alpha, start_yaw, dest_yaw, pitch, yaw1, yaw2;
	Fvector				direction, temp;
	
	// computing 2D cross product for start point
	direction.sub		(start.position,start_circle.center);
	if (fis_zero(direction.square_magnitude()))
		direction		= start.direction;
	direction.getHP		(start_yaw,pitch);
	start_yaw			= angle_normalize(start_yaw);
	start_cp			= cross_product_2D_y(start.direction,direction);
	
	// computing 2D cross product for dest point
	direction.sub		(dest.position,dest_circle.center);
	if (fis_zero(direction.square_magnitude()))
		direction		= dest.direction;
	direction.getHP		(dest_yaw,pitch);
	dest_yaw			= angle_normalize(dest_yaw);
	dest_cp				= cross_product_2D_y(dest.direction,direction);

	// direction from the first circle to the second one
	direction.sub		(dest_circle.center,start_circle.center);
	direction.getHP		(yaw1,pitch);
	yaw1 = yaw2			= angle_normalize(yaw1);

	if (start_cp*dest_cp >= 0.f) {
		// so, our tangents are outside
		if (start_circle.center.similar(dest_circle.center)) {
			if  (fsimilar(start_circle.radius,dest_circle.radius)) {
				// so, our circles are equal
				tangents[0]			= tangents[1] = start_circle;
				if (start_cp >= 0.f) {
					adjust_point	(start_circle.center,yaw1 + PI_DIV_2,	start_circle.radius,tangents[0].point);
					assign_angle	(tangents[0].angle,start_yaw,angle_normalize(yaw1 + PI_DIV_2),true);
				}
				else {
					adjust_point	(start_circle.center,yaw1 - PI_DIV_2,	start_circle.radius,tangents[0].point);
					assign_angle	(tangents[0].angle,start_yaw,angle_normalize(yaw1 - PI_DIV_2),false);
				}

				tangents[1].point	= tangents[0].point;
				tangents[1].angle	= 0.f;
				TIMER_STOP(ComputeTangent)
				return				(true);
			}
			else {
				TIMER_STOP(ComputeTangent)
				return				(false);
			}
		}
		else {
			// distance between circle centers
			distance		= start_circle.center.distance_to_xz(dest_circle.center);
			// radius difference
			float			r_diff = start_circle.radius - dest_circle.radius;
			// angle between external tangents and circle centers segment
			alpha			= angle_normalize(acosf(r_diff/distance));
		}
	}
	else {
		distance		= start_circle.center.distance_to_xz(dest_circle.center);
		// so, our tangents are inside (crossing)
		if (start_circle.radius + dest_circle.radius >= distance) {
			TIMER_STOP(ComputeTangent)
			return		(false);
		}
	
		// angle between internal tangents and circle centers segment
		alpha			= angle_normalize(acosf((start_circle.radius + dest_circle.radius)/distance));
		yaw2			= angle_normalize(yaw1 + PI);
	}

	tangents[0]			= start_circle;
	tangents[1]			= dest_circle;
	start_yaw			= angle_normalize(start_yaw);
	dest_yaw			= angle_normalize(dest_yaw);

	// compute external tangent points
	adjust_point		(start_circle.center,yaw1 + alpha,	start_circle.radius,tangents[0].point);
	adjust_point		(dest_circle.center, yaw2 + alpha,	dest_circle.radius, tangents[1].point);

	direction.sub		(tangents[1].point,tangents[0].point);
	temp.sub			(tangents[0].point,start_circle.center);
	float				tangent_cp = cross_product_2D_y(direction,temp);
	if (start_cp*tangent_cp >= 0) {
		assign_angle	(tangents[0].angle,start_yaw,angle_normalize(yaw1 + alpha),start_cp >= 0);
		assign_angle	(tangents[1].angle,dest_yaw, angle_normalize(yaw2 + alpha),dest_cp  >= 0,false);
		TIMER_STOP(ComputeTangent)
		return			(true);
	}

	// compute external tangent points
	adjust_point		(start_circle.center,yaw1 - alpha,	start_circle.radius,tangents[0].point);
	adjust_point		(dest_circle.center, yaw2 - alpha,	dest_circle.radius, tangents[1].point);
	assign_angle		(tangents[0].angle,start_yaw,angle_normalize(yaw1 - alpha),start_cp >= 0);
	assign_angle		(tangents[1].angle,dest_yaw, angle_normalize(yaw2 - alpha),dest_cp  >= 0,false);

	TIMER_STOP(ComputeTangent)
	return				(true);
}

IC	bool build_circle_trajectory(
	CLevelGraph							&level_graph,
	const STrajectoryPoint				&position, 
	xr_vector<Fvector>					*path,
	bool								start_point
)
{
	TIMER_START(BuildCircleTrajectory)
	Fvector				direction, curr_pos;
	u32					curr_vertex_id;
	if (start_point) {
		direction.sub	(position.position,position.center);
		curr_pos		= position.position;
		curr_vertex_id	= position.vertex_id;
	}
	else {
		direction.sub	(position.point,position.center);
		curr_pos		= position.point;
		TIMER_START(BCT_CPID)
		curr_vertex_id	= level_graph.check_position_in_direction(position.vertex_id,position.position,curr_pos);
		TIMER_STOP(BCT_CPID)
		if (!level_graph.valid_vertex_id(curr_vertex_id)) {
			TIMER_STOP(BuildCircleTrajectory)
			return		(false);
		}
		curr_pos.y		= level_graph.vertex_plane_y(curr_vertex_id,curr_pos.x,curr_pos.z);
	}

	float				yaw,pitch;
	direction.getHP		(yaw,pitch);
	yaw					= angle_normalize(yaw);
	u32					m = iFloor(_abs(position.angle)/position.angular_velocity*10.f +.5f);
	for (u32 i=start_point ? 0 : 1, n=fis_zero(position.angular_velocity) ? 1 : m; i<=n; ++i) {
		Fvector			t;
		adjust_point	(position.center,yaw + float(i)*position.angle/float(n),position.radius,t);
		TIMER_START(BCT_CPID)
		curr_vertex_id	= level_graph.check_position_in_direction(curr_vertex_id,curr_pos,t);
		TIMER_STOP(BCT_CPID)
		if (!level_graph.valid_vertex_id(curr_vertex_id)) {
			TIMER_STOP(BuildCircleTrajectory)
			return		(false);
		}
		if (path) {
			t.y			= level_graph.vertex_plane_y(curr_vertex_id,t.x,t.z);
			path->push_back(t);
		}
		curr_pos		= t;
	}
	TIMER_STOP(BuildCircleTrajectory)
	return				(true);
}

IC	bool build_line_trajectory(
	CLevelGraph							&level_graph,
	const STrajectoryPoint				&start, 
	const STrajectoryPoint				&dest, 
	xr_vector<Fvector>					*path
)
{
	TIMER_START(BuildLineTrajectory)
	xr_vector<u32>		node_path;
	if (level_graph.inside(start.vertex_id,dest.point)) {
		if (path)
			path->push_back	(dest.point);
		TIMER_STOP(BuildLineTrajectory)
		return			(true);
	}
	u32					vertex_id = level_graph.check_position_in_direction(start.vertex_id,start.position,start.point);
	if (!level_graph.valid_vertex_id(vertex_id)) {
		TIMER_STOP(BuildLineTrajectory)
		return			(false);
	}
	bool				b = path ? level_graph.create_straight_PTN_path(vertex_id,start.point,dest.point,*path,node_path,false,false) : level_graph.valid_vertex_id(level_graph.check_position_in_direction(vertex_id,start.point,dest.point));
	TIMER_STOP(BuildLineTrajectory)
	return				(b);
}

IC	bool build_trajectory(
	CLevelGraph							&level_graph,
	const STrajectoryPoint				&start, 
	const STrajectoryPoint				&dest, 
	xr_vector<Fvector>					*path
)
{
	TIMER_START(BuildTrajectory2)
	if (!build_circle_trajectory(level_graph,start,path,true)) {
//		Msg				("FALSE : Circle 0");
		TIMER_STOP(BuildTrajectory2)
		return			(false);
	}
	if (!build_line_trajectory(level_graph,start,dest,path)) {
//		Msg				("FALSE : Line");
		TIMER_STOP(BuildTrajectory2)
		return			(false);
	}
	if (!build_circle_trajectory(level_graph,dest,path,false)) {
//		Msg				("FALSE : Circle 1");
		TIMER_STOP(BuildTrajectory2)
		return			(false);
	}
//	Msg					("TRUE");
	TIMER_STOP(BuildTrajectory2)
	return				(true);
}

struct SDist {
	u32		index;
	float	time;

	bool operator<(const SDist &d1) const
	{
		return		(time < d1.time);
	}
};

IC	bool build_trajectory(
	CLevelGraph							&level_graph,
	STrajectoryPoint					&start, 
	STrajectoryPoint					&dest, 
	const SCirclePoint					tangents[4][2], 
	const u32							tangent_count,
	float								straight_velocity,
	xr_vector<Fvector>					*path,
	float								&time
)
{
	TIMER_START(BuildTrajectory1)
	time			= flt_max;
	SDist			dist[4];
	{
		for (u32 i=0; i<tangent_count; ++i) {
			dist[i].index = i;
			dist[i].time = 
				_abs(tangents[i][0].angle)/start.angular_velocity +
				_abs(tangents[i][1].angle)/dest.angular_velocity +
				tangents[i][0].point.distance_to_xz(tangents[i][1].point)*
				(fis_zero(straight_velocity) ? 0 : 1.f/straight_velocity); 
		}
	}
	
	std::sort	(dist,dist + tangent_count);

	{
		for (u32 i=0, j = path ? path->size() : 0; i<tangent_count; ++i) {
			(SCirclePoint&)(start) = tangents[dist[i].index][0];
			(SCirclePoint&)(dest)	= tangents[dist[i].index][1];
			if (build_trajectory(level_graph,start,dest,path)) {
				time	= dist[i].time;
				TIMER_STOP(BuildTrajectory1)
				return	(true);
			}
			else
				if (path)
					path->resize(j);
		}
	}

	TIMER_STOP(BuildTrajectory1)
	return		(false);
}

IC	bool compute_trajectory(
	CLevelGraph							&level_graph,
	STrajectoryPoint					&start,
	STrajectoryPoint					&dest,
	float								straight_velocity,
	xr_vector<Fvector>					*path,
	float								&time
)
{
	TIMER_START(ComputeTrajectory)
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

	bool			b = build_trajectory(level_graph,start,dest,tangent_points,tangent_count,straight_velocity,path,time);
	TIMER_STOP(ComputeTrajectory)
	return			(b);
}

bool compute_path(
	CLevelGraph							&level_graph,
	STrajectoryPoint					&start,
	STrajectoryPoint					&dest,
	xr_vector<STravelParams>			&start_set,
	xr_vector<STravelParams>			&dest_set,
	xr_vector<Fvector>					*m_tpTravelLine
)
{
	TIMER_START(ComputePath)
	xr_vector<Fvector>		travel_line;
	float					min_time = flt_max, time;
	u32						size = m_tpTravelLine ? m_tpTravelLine->size() : 0;
	xr_vector<STravelParams>::const_iterator I = start_set.begin(), B = I;
	xr_vector<STravelParams>::const_iterator E = start_set.end();
	for ( ; I != E; ++I) {
		(STravelParams&)start	= *I;
		xr_vector<STravelParams>::const_iterator i = dest_set.begin(), b = i;
		xr_vector<STravelParams>::const_iterator e = dest_set.end();
		for ( ; i != e; ++i) {
			(STravelParams&)dest	= *i;
			travel_line.clear				();
			if (compute_trajectory(level_graph,start,dest,6.f,m_tpTravelLine ? &travel_line : 0,time)) {
//				Msg		("[L=%f][A=%f][L=%f][A=%f] : %f",
//					start.linear_velocity,
//					start.angular_velocity,
//					dest.linear_velocity,
//					dest.angular_velocity,
//					time
//				);
				if (time < min_time) {
					min_time = time;
					if (m_tpTravelLine) {
						m_tpTravelLine->resize(size);
						m_tpTravelLine->insert(m_tpTravelLine->end(),travel_line.begin(),travel_line.end());
					}
					else {
						TIMER_STOP(ComputePath)
						return(true);
					}
				}
			}
		}
	}
	
	if (fsimilar(min_time,flt_max)) {
		TIMER_STOP			(ComputePath)
		return				(false);
	}
	
	TIMER_STOP				(ComputePath)
	return					(true);
}

void build_detail_path(
	CLevelGraph					&level_graph, 
	STrajectoryPoint			&start, 
	STrajectoryPoint			&dest,
	xr_vector<STravelParams>	&start_set,
	xr_vector<STravelParams>	&dest_set,
	xr_vector<u32>				&m_tpaNodes
)
{
	xr_vector<Fvector>						m_tpTravelLine;
	xr_vector<Fvector>						travel_line;
	xr_vector<STravelPoint>					key_points;
	STrajectoryPoint						s,d,t,p;

	m_tpTravelLine.clear					();

	VERIFY									(!m_tpaNodes.empty());
	if (m_tpaNodes.size() == 1) {
		if (!compute_path(level_graph,start,dest,start_set,dest_set,&m_tpTravelLine)) {
			m_tpTravelLine.clear			();
			return;
		}
	}
	else {
		if (compute_path(level_graph,start,dest,start_set,dest_set,&m_tpTravelLine)) {
			return;
		}
		
		m_tpTravelLine.clear				();

		STravelPoint						start_point;
		start_point.vertex_id				= m_tpaNodes.front();
		start_point.position				= start.position;

		for (int _i=0, i=0, n=(int)m_tpaNodes.size() - 1, j = n, m=j; _i < n; ) {
			if (!level_graph.check_vertex_in_direction(start_point.vertex_id,start_point.position,m_tpaNodes[j])) {
				m							= j;
				j							= (i + j)/2;
			}
			else {
				i							= j;
				j							= (i + m)/2;
			}
			if (i >= m - 1) {
				if (i <= _i)
					return;
				_i							= i;
				key_points.push_back		(start_point);
				if (i == n) {
					if (level_graph.valid_vertex_id(level_graph.check_position_in_direction(start_point.vertex_id,start_point.position,dest.position))) {
						key_points.push_back(dest);
						break;
					}
					else
						return;
				}
				start_point.vertex_id		= m_tpaNodes[_i];
				start_point.position		= level_graph.vertex_position(start_point.vertex_id);
				j = m						= n;
			}
		}

		s = t								= start;
		xr_vector<STravelPoint>::const_iterator	I = key_points.begin(), P = I;
		xr_vector<STravelPoint>::const_iterator	E = key_points.end();
		for ( ; I != E; ++I) {
			// setting up destination
			if ((I + 1) != E) {
				(STravelPoint&)d = *I;
				d.direction.sub				((I + 1)->position,d.position);
				VERIFY						(!fis_zero(d.direction.magnitude()));
			}
			else
				d							= dest;

			if (!compute_path(level_graph,s,d,start_set,dest_set,0)) {
				if (I == P) {
					m_tpTravelLine.clear	();
					return;
				}
				
				p							= d;
				(STravelPoint&)d = *(I - 1);
				d.direction.sub				((*I).position,d.position);
				VERIFY						(!fis_zero(d.direction.magnitude()));
				if (!compute_path(level_graph,s,d,start_set,dest_set,&m_tpTravelLine)) {
					VERIFY					(false);
				}
				P							= I - 1;
				d							= p;
				s							= t;
				VERIFY						(!fis_zero(s.direction.magnitude()));
				if (!compute_path(level_graph,s,d,start_set,dest_set,0)) {
					m_tpTravelLine.clear	();
					return;
				}
			}
			t								= d;
		}
		if (!compute_path(level_graph,s,d,start_set,dest_set,&m_tpTravelLine)) {
			if (compute_path(level_graph,s,d,start_set,dest_set,0)) {
				compute_path(level_graph,s,d,start_set,dest_set,&m_tpTravelLine);
				t=t;
			}
			VERIFY							(false);
		}
	}
}

void fill_params(
	STrajectoryPoint			&start,
	STrajectoryPoint			&dest,
	xr_vector<STravelParams>	&start_set,
	xr_vector<STravelParams>	&dest_set
)
{
	start.angular_velocity	= PI_DIV_2;
	start.linear_velocity	= 0.f;//0.0001f;
	start_set.push_back		(start);

	start.angular_velocity	= PI;
	start.linear_velocity	= 2.15f;
	start_set.push_back		(start);

	start.angular_velocity	= PI_DIV_2;
	start.linear_velocity	= 4.5f;
	start_set.push_back		(start);

	start.angular_velocity	= PI_DIV_4;
	start.linear_velocity	= 6.f;
	start_set.push_back		(start);

	dest.angular_velocity	= PI_DIV_2;
	dest.linear_velocity	= 0.f;
	dest_set.push_back		(dest);

	dest.angular_velocity	= PI;
	dest.linear_velocity	= 2.15f;
	dest_set.push_back		(dest);

	dest.angular_velocity	= PI_DIV_2;
	dest.linear_velocity	= 4.5f;
	dest_set.push_back		(dest);

	dest.angular_velocity	= PI_DIV_4;
	dest.linear_velocity	= 6.f;
	dest_set.push_back		(dest);
}

#define TEST_COUNT 1000

void test_smooth_path(LPCSTR name)
{
	init_timers					();

	STrajectoryPoint			start,dest;
	xr_vector<STravelParams>	start_set, dest_set;
	CLevelGraph					*level_graph = xr_new<CLevelGraph>(name);
	CGraphEngine				*graph_engine = xr_new<CGraphEngine>(level_graph->header().vertex_count());
	xr_vector<u32>				m_tpaNodes;
	
	start.position.set			(-30.799995f,-0.000439f,-15.400002f);
	start.direction.set			(35.700005f,-0.021492f,0.000000);
	start.vertex_id				= level_graph->vertex(start.position);

	dest.position.set			(5.010808f,-0.000005f,3.826570f);
	dest.direction.set			(-0.012606f,0.000000f,-0.999921f);
	dest.vertex_id				= level_graph->vertex(dest.position);
	
	fill_params					(start,dest,start_set,dest_set);

	if (!graph_engine->search(*level_graph,start.vertex_id,dest.vertex_id,&m_tpaNodes,CGraphEngine::CBaseParameters()))
		return;

	RESET_ALL_TIMERS;

//	SetPriorityClass			(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
//	SetThreadPriority			(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	Sleep						(1);
	
	TIMER_START					(BuildDetailPath);
	for (int i=0; i<TEST_COUNT; ++i) {
		build_detail_path		(*level_graph,start,dest,start_set,dest_set,m_tpaNodes);
	}
	TIMER_STOP					(BuildDetailPath);
	
//	SetThreadPriority			(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
//	SetPriorityClass			(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);

	PRINT_TIMERS;

	xr_delete					(level_graph);
	xr_delete					(graph_engine);
}