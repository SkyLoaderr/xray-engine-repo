#pragma once

#include "r1_light.h"

// refs
class ENGINE_API IReader;
class ENGINE_API CObject;
class CLightTrack;

// t-defs
class CLightDB_Static 
{
	DEF_VECTOR(vecI,int);

	xr_vector<R1_light*>		Lights;			// -- Lights itself
	vecI						Selected;		// Selected (static only) in one frame
public:
	void	add_light			(WORD L);

	void	UnselectAll			(void);			// Disables all lights

	void	Track				(IRenderable* O);

	void	Load				(IReader* fs);
	void	Unload				(void);
	void	Render				(void);
};
