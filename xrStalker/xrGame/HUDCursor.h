#ifndef __XR_HUDCURSOR_H__
#define __XR_HUDCURSOR_H__
#pragma once

#include "HUDCrosshair.h"


class CHUDManager;

class CHUDCursor
{
	friend class CHUDManager;

	ref_shader				hShader;
	ref_geom				hGeom;
	float					fuzzyShowInfo;
	Collide::rq_result		RQ;

	bool					m_bShowCrosshair;
	CHUDCrosshair			HUDCrosshair;
public:
							CHUDCursor	();
							~CHUDCursor	();

	void					Render		();
	void					Load		();
};

#endif //__XR_HUDCURSOR_H__
