#ifndef __XR_HUDCURSOR_H__
#define __XR_HUDCURSOR_H__
#pragma once

class CHUDManager;

class CHUDCursor
{
	friend class CHUDManager;

	ref_shader				hShader;
	ref_geom				hGeom;
	float					fuzzyShowInfo;
	Collide::rq_result		RQ;
public:
							CHUDCursor	();
							~CHUDCursor	();
	void					Render		();
};

#endif //__XR_HUDCURSOR_H__
