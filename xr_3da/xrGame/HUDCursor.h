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
#pragma todo("Dima to Jim : check if you need here a non-virtual destructor!")
							~CHUDCursor	();
	void					Render		();
};

#endif //__XR_HUDCURSOR_H__
