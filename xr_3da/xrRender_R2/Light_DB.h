#pragma once

#include "..\light.h"
#include "light.h"

class CLight_DB
{
	Fvector3				sun_dir;
	Fvector4				sun_color;

	vector<xrLIGHT_control>	v_static_controls;
	vector<light*>			v_static;
	set<light*>				v_dynamic_active;
	set<light*>				v_dynamic_inactive;

	vector<light*>			v_selected_shadowed;
	vector<light*>			v_selected_unshadowed;
public:
	void					add_sector_lights	(vector<WORD> &L);

	void					Load				(IReader* fs);
	void					Unload				();

	void					Activate			(light* L);
	void					Deactivate			(light* L);

	light*					Create				();
	void					Destroy				(light*);

	CLight_DB				();
	~CLight_DB				();
};
