#pragma once

IC	int	CCustomMonster::ifGetMemberIndex() 
{
	xr_vector<CEntity*> &tpaMembers = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()].Members;
	int iCount = (int)tpaMembers.size();
	for (int i=0; i<iCount; ++i)
		if (this == tpaMembers[i])
			return(i);
	return(-1);
};

IC	bool CCustomMonster::angle_lerp_bounds(float &a, float b, float c, float d)
{
	if (c*d >= angle_difference(a,b)) {
		a = b;
		return(true);
	}
	
	angle_lerp(a,b,c,d);

	return(false);
};

IC  CGroup *CCustomMonster::getGroup()
{
	return	(&(Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()]));
}

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
