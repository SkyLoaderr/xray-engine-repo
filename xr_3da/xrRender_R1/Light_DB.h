#pragma once

#include "light.h"
#include "light_package.h"

class	CLight_DB
{
private:
	xr_vector<light*>		v_static;
public:
	light*					sun_original;
	light*					sun_adapted;
	light_Package			package;
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
