#pragma once

#include "light.h"

class CLight_DB
{
public:
	Fvector3				sun_dir;
	Fvector3				sun_color;
private:
	xr_vector<light*>		v_static;
public:
	xr_vector<light*>		v_selected_shadowed;
	xr_vector<light*>		v_selected_unshadowed;
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
