#pragma once

#include "light.h"

class CLight_DB
{
public:
	Fvector3				sun_dir;
	Fvector3				sun_color;
private:
	xr_vector<light*>		v_static;
	xr_set<light*>			v_dynamic_active;
	xr_set<light*>			v_dynamic_inactive;
public:
	xr_vector<light*>		v_selected_shadowed;
	xr_vector<light*>		v_selected_unshadowed;
public:
	void					add_sector_lights	(xr_vector<WORD> &	L);
	void					add_sector_dlight	(light*			L);

	void					Load				(IReader* fs);
	void					Unload				();

	void					Activate			(light* L);
	void					Deactivate			(light* L);

	light*					Create				();
	void					Destroy				(light*);
	void					Update				();

	CLight_DB				();
	~CLight_DB				();
};
