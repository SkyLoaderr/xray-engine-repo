#ifndef __XR_HITMARKER_H__
#define __XR_HITMARKER_H__
#pragma once

class CHitMarker
{
public:
	float					fHitMarks[4];
	ref_shader				hShader;
	ref_geom				hGeom;
public:
							CHitMarker	();
							~CHitMarker	();

	void					Render		();
	void					Hit			(int id);
};

#endif // __XR_HITMARKER_H__
