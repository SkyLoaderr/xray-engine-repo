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

IC	bool left_angle(float y1, float y2)
{
	Fvector			dir, dir1, dir2;
	dir1.setHP		(y1,0.f);
	dir2.setHP		(y2,0.f);
	dir.crossproduct(dir1,dir2);
	return			(dir.y <= 0.f);
}
