#ifndef __XR_HUDCURSOR_H__
#define __XR_HUDCURSOR_H__
#pragma once

class CHUDCursor
{
	ref_shader				hShader;
	ref_geom				hGeom;
	float					fuzzyShowInfo;
public:
							CHUDCursor	();
							~CHUDCursor	();
	void					Render		();
};

#endif //__XR_HUDCURSOR_H__
