////////////////////////////////////////////////////////////////////////////
//	Module 		: detailed_path_manager_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Detail path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	bool CDetailPathManager::failed() const
{
	return					(m_failed);
}

IC	bool CDetailPathManager::completed(const Fvector &position) const
{
//	return					(m_path.empty() || (m_dest_position.similar(m_path.back().position) && position.similar(m_dest_position,.15f)));
	return					(m_path.empty() || (curr_travel_point_index() == m_path.size() - 1));
}

IC	const xr_vector<CDetailPathManager::STravelPathPoint> &CDetailPathManager::path() const
{
	return					(m_path);
}

IC	const CDetailPathManager::STravelPathPoint &CDetailPathManager::curr_travel_point() const
{
	return					(m_path[curr_travel_point_index()]);
}

IC	u32	 CDetailPathManager::curr_travel_point_index() const
{
	VERIFY					(!m_path.empty() && (m_current_travel_point < m_path.size()));
	return					(m_current_travel_point);
}

IC	void CDetailPathManager::set_start_position	(const Fvector &start_position)
{
	m_start_position		= start_position;
}

IC	void CDetailPathManager::set_start_direction	(const Fvector &start_direction)
{
	m_start_direction		= start_direction;
}

IC	void CDetailPathManager::set_dest_position	(const Fvector &dest_position)
{
	m_actuality				= m_actuality && m_dest_position.similar(dest_position);
	m_dest_position			= dest_position;
}

IC	void CDetailPathManager::set_dest_direction	(const Fvector &dest_direction)
{
	m_actuality				= m_actuality && m_dest_direction.similar(dest_direction);
	m_dest_direction		= dest_direction;
}

IC	const Fvector &CDetailPathManager::start_position	() const
{
	return					(m_start_position);
}

IC	const Fvector &CDetailPathManager::start_direction	() const
{
	return					(m_start_direction);
}

IC	const Fvector &CDetailPathManager::dest_position	() const
{
	return					(m_dest_position);
}

IC	const Fvector &CDetailPathManager::dest_direction	() const
{
	return					(m_dest_direction);
}

IC	void CDetailPathManager::set_path_type				(const EDetailPathType path_type)
{
	m_actuality				= m_actuality && (path_type == m_path_type);
	m_path_type				= path_type;
}

IC	void CDetailPathManager::adjust_point(
	const Fvector2		&source, 
	float				yaw, 
	float				magnitude, 
	Fvector2			&dest
) const
{
	dest.x				= -_sin(yaw);
	dest.y				= _cos(yaw);
	dest.mad			(source,dest,magnitude);
}

IC	void CDetailPathManager::assign_angle(
	float				&angle, 
	const float			start_yaw, 
	const float			dest_yaw, 
	const bool			positive,
	const bool			start
) const
{
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
			angle = angle + PI_MUL_2;
		else
			angle = angle - PI_MUL_2;

	VERIFY				(_valid(angle));
}

IC	void CDetailPathManager::compute_circles(
	STrajectoryPoint	&point, 
	SCirclePoint		*circles
)
{
	VERIFY				(!fis_zero(point.angular_velocity));
	point.radius		= point.linear_velocity/point.angular_velocity;
	circles[0].radius	= circles[1].radius = point.radius;
	VERIFY				(fsimilar(point.direction.square_magnitude(),1.f));
	circles[0].center.x =  point.direction.y*point.radius + point.position.x;
	circles[0].center.y = -point.direction.x*point.radius + point.position.y;
	circles[1].center.x = -point.direction.y*point.radius + point.position.x;
	circles[1].center.y =  point.direction.x*point.radius + point.position.y;
}

IC	const u32 CDetailPathManager::desirable_speed		() const
{
	return					(m_desirable_velocity);
}

IC	void CDetailPathManager::set_desirable_speed	(const u32 desirable_speed)
{
	m_actuality				= m_actuality && (!try_desirable_speed() || (desirable_speed == m_desirable_velocity));
	m_desirable_velocity	= desirable_speed;
}

IC	const float CDetailPathManager::desirable_linear_speed	() const
{
	xr_map<u32,STravelParams>::const_iterator	I = m_movement_params.find(desirable_speed());
	VERIFY					(m_movement_params.end() != I);
	return					((*I).second.linear_velocity);
}

IC	const float	CDetailPathManager::desirable_angular_speed	() const
{
	xr_map<u32,STravelParams>::const_iterator	I = m_movement_params.find(desirable_speed());
	VERIFY					(m_movement_params.end() != I);
	return					((*I).second.angular_velocity);
}

IC	void CDetailPathManager::set_velocity_mask			(const u32 velocity_mask)
{
	m_actuality				= m_actuality && (velocity_mask == m_velocity_mask);
	m_velocity_mask			= velocity_mask;
}

IC	const u32 CDetailPathManager::velocity_mask			() const
{
	return					(m_velocity_mask);
}

IC	void CDetailPathManager::set_try_min_time			(const bool try_min_time)
{
	m_actuality				= m_actuality && (try_min_time == m_try_min_time);
	m_try_min_time			= try_min_time;
}

IC	const bool CDetailPathManager::try_min_time			() const
{
	return					(m_try_min_time);
}

IC	void CDetailPathManager::set_try_desirable_speed	(const bool try_desirable_speed)
{
	m_actuality				= m_actuality && (try_desirable_speed == m_try_desirable_speed);
	m_try_desirable_speed	= try_desirable_speed;
}

IC	const bool CDetailPathManager::try_desirable_speed	() const
{
	return					(m_try_desirable_speed);
}

IC	void CDetailPathManager::set_use_dest_orientation	(const bool use_dest_orientation)
{
	m_actuality				= m_actuality && (use_dest_orientation == m_use_dest_orientation);
	m_use_dest_orientation	= use_dest_orientation;
}

IC	const bool CDetailPathManager::use_dest_orientation		() const
{
	return					(m_use_dest_orientation);
}

IC	bool CDetailPathManager::check_mask					(u32 mask, u32 test) const
{
	return					((mask & test) == test);
}