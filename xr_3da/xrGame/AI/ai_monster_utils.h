#pragma once


// проверить, находится ли объект entity на ноде
// возвращает позицию объекта, если он находится на ноде, или центр его ноды
IC Fvector get_valid_position(const CEntity *entity, const Fvector &actual_position) 
{
	if (ai().level_graph().inside(entity->level_vertex(), actual_position)) return actual_position;
	else return ai().level_graph().vertex_position(entity->level_vertex());
}

// возвращает true, если объект entity находится на ноде
IC bool object_position_valid(const CEntity *entity)
{
	return ai().level_graph().inside(entity->level_vertex_id(), entity->Position());
}

IC Fvector random_position(const Fvector &center, float R) 
{
	Fvector v;
	v = center;
	v.x += ::Random.randF(-R,R);
	v.z += ::Random.randF(-R,R);

	return v;
}

IC bool	from_right(float ty, float cy) 
{
	return ((angle_normalize_signed(ty - cy) > 0));
}

IC bool	is_angle_between(float yaw, float yaw_from, float yaw_to)
{
	float diff = angle_difference(yaw_from,yaw_to);
	R_ASSERT(diff < PI);

	if ((angle_difference(yaw,yaw_from) < diff) && (angle_difference(yaw,yaw_to)<diff)) return true;
	else return false;
}


