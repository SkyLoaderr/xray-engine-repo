#pragma once

#include "HUDCrosshair.h"
#include "../xr_collide_defs.h"


class CHUDManager;

class CHUDTarget
{
	friend class CHUDManager;

	ref_shader				hShader;
	ref_geom				hGeom;
	float					fuzzyShowInfo;
	collide::rq_result		RQ;

	bool					m_bShowCrosshair;
	CHUDCrosshair			HUDCrosshair;
public:
							CHUDTarget	();
							~CHUDTarget	();
	void					CursorOnFrame ();
	void					Render		();
	void					Load		();
};
