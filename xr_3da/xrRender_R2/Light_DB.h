#pragma once

#include "light.h"

class CLight_DB
{
	vector<light*>			v_static;
	set<light*>				v_dynamic_active_shadow;
	set<light*>				v_dynamic_active_noshadow;
	set<light*>				v_dynamic_inactive;
	vector<light*>			v_selected;
public:
	void					add_sector_lights	(vector<WORD> &L);

	void					Load				(IReader* fs);
	void					Unload				(void);

	light*					Create				();
	void					Destroy				(light*);

	CLight_DB				();
	~CLight_DB				();
};
