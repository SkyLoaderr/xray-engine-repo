#pragma once

#include "light.h"

class CLight_DB
{
public:
	void					Load			(IReader* fs);
	void					Unload			(void);

	light*					Create			();
	void					Destroy			(light*);

	CLight_DB				();
	~CLight_DB				();
};
