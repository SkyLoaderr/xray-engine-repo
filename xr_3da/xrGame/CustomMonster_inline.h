#pragma once

IC	bool CCustomMonster::angle_lerp_bounds(float &a, float b, float c, float d)
{
	if (c*d >= angle_difference(a,b)) {
		a = b;
		return(true);
	}
	
	angle_lerp(a,b,c,d);

	return(false);
};

IC void CCustomMonster::vfNormalizeSafe(Fvector& Vector)
{
	float fMagnitude = Vector.magnitude(); 
	if (fMagnitude > EPS_L) {
		Vector.x /= fMagnitude;
		Vector.y /= fMagnitude;
		Vector.z /= fMagnitude;
	}
	else {
		Vector.x = 1.f;
		Vector.y = 0.f;
		Vector.z = 0.f;
	}
}

ICF	bool left_angle(float y1, float y2)
{
	return			(_sin(y1)*_cos(y2) - _sin(y2)*_cos(y1) <= 0.f);
}

IC	CMemoryManager &CCustomMonster::memory		() const
{
	VERIFY			(m_memory_manager);
	return			(*m_memory_manager);
}

IC	CMovementManager &CCustomMonster::movement	() const
{
	VERIFY			(m_movement_manager);
	return			(*m_movement_manager);
}

IC	CSoundPlayer &CCustomMonster::sound			() const
{
	VERIFY			(m_sound_player);
	return			(*m_sound_player);
}

IC	CMaterialManager &CCustomMonster::material	() const
{
	VERIFY			(m_material_manager);
	return			(*m_material_manager);
}

IC	CSound_UserDataVisitor *CCustomMonster::sound_user_data_visitor	() const
{
	VERIFY			(m_sound_user_data_visitor);
	return			(m_sound_user_data_visitor);
}
