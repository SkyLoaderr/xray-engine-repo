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
	return					(m_path.empty() || (m_dest_position.similar(m_path.back().position) && position.similar(m_dest_position,.15f)));
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

IC	void CDetailPathManager::set_dest_position	(const Fvector &dest_position)
{
	m_actuality				= m_actuality && m_dest_position.similar(dest_position);
	m_dest_position			= dest_position;
}

IC	const Fvector &CDetailPathManager::start_position	() const
{
	return					(m_start_position);
}

IC	const Fvector &CDetailPathManager::dest_position	() const
{
	return					(m_dest_position);
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

