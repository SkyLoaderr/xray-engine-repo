#pragma once

class	light_Package
{
public:
	xr_vector<light*>		v_point;
	xr_vector<light*>		v_point_s;
	xr_vector<light*>		v_spot;
	xr_vector<light*>		v_spot_s;
public:
	void					clear				()
	{
		v_point.clear		();
		v_point_s.clear		();
		v_spot.clear		();
		v_spot_s.clear		();
	}
};
