#pragma once

#include "..\light.h"
#include "light.h"

class CLight_DB
{
public:
	Fvector3				sun_dir;
	Fvector3				sun_color;
private:
	vector<xrLIGHT_control>	v_static_controls;
	vector<light*>			v_static;
	set<light*>				v_dynamic_active;
	set<light*>				v_dynamic_inactive;
public:
	vector<light*>			v_selected_shadowed;
	vector<light*>			v_selected_unshadowed;
public:
	void					add_sector_lights	(vector<WORD> &	L);
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
