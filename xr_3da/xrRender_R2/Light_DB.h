#pragma once

#include "light.h"

class CLight_DB
{
private:
	xr_vector<light*>		v_static;
public:
	light*					sun;
	light*					sun_base;
	xr_vector<light*>		v_point;
	xr_vector<light*>		v_point_s;
	xr_vector<light*>		v_spot;
	xr_vector<light*>		v_spot_s;
public:
	void					add_light			(light*		L);

	void					Load				(IReader*	fs);
	void					Unload				();

	light*					Create				();
	void					Destroy				(light*);
	void					Update				();

	CLight_DB				();
	~CLight_DB				();
};
