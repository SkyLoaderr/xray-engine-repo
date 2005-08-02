#pragma once

// ���������, ��������� �� ������ entity �� ����
// ���������� ������� �������, ���� �� ��������� �� ����, ��� ����� ��� ����
extern Fvector get_valid_position(const CEntity *entity, const Fvector &actual_position);

// ���������� true, ���� ������ entity ��������� �� ����
extern bool object_position_valid(const CEntity *entity);

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

IC void velocity_lerp(float &_cur, float _target, float _accel, float _dt)
{
	if (fsimilar(_cur, _target)) return;

	if (_target > _cur) {
		_cur += _accel * _dt;
		if (_cur > _target) _cur = _target;
	} else {
		_cur -= _accel * _dt;
		if (_cur < 0) _cur = 0.f;
	}
}

IC void def_lerp(float &_cur, float _target, float _vel, float _dt)
{
	if (fsimilar(_cur, _target)) return;

	if (_target > _cur) {
		_cur += _vel * _dt;
		if (_cur > _target) _cur = _target;
	} else {
		_cur -= _vel * _dt;
		if (_cur < _target) _cur = _target;
	}
}

IC u32	time() 
{
	return Device.dwTimeGlobal;
}