#pragma once
/*
IC	bool 
CHelicopterMovementManager::check_mask					(u32 mask, u32 test) const
{
	return					((mask & test) == test);
}
*/

IC	void 
CHelicopterMovementManager::compute_circles(
	STrajectoryPoint	&point, 
	SCirclePoint		*circles
	)
{
	VERIFY				(!fis_zero(point.angular_velocity));
	point.radius		= _abs(point.linear_velocity)/point.angular_velocity;
	circles[0].radius	= circles[1].radius = point.radius;
	VERIFY				(fsimilar(point.direction.square_magnitude(),1.f));
	circles[0].center.x =  point.direction.y*point.radius + point.position.x;
	circles[0].center.y = -point.direction.x*point.radius + point.position.y;
	circles[1].center.x = -point.direction.y*point.radius + point.position.x;
	circles[1].center.y =  point.direction.x*point.radius + point.position.y;
}


IC	void 
CHelicopterMovementManager::adjust_point(
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

IC	void 
CHelicopterMovementManager::assign_angle(
	float					&angle, 
	const float				start_yaw, 
	const float				dest_yaw, 
	const bool				positive,
	const EDirectionType	direction_type,
	const bool				start
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

	if (!start && ((direction_type == eDirectionTypePP) || (direction_type == eDirectionTypeNN)))
		if (angle < 0.f)
			angle = angle + PI_MUL_2;
		else
			angle = angle - PI_MUL_2;

	VERIFY				(_valid(angle));
}
