#ifndef __XR_HUDCURSOR_H__
#define __XR_HUDCURSOR_H__
#pragma once

class CHUDCursor : public pureDeviceDestroy, public pureDeviceCreate
{
	ref_shader				hShader;
	ref_geom				hGeom;
	float					fuzzyShowInfo;
public:
							CHUDCursor	();
							~CHUDCursor	();
	void					Render		();

	virtual void			OnDeviceDestroy	();
	virtual void			OnDeviceCreate	();
};

#endif //__XR_HUDCURSOR_H__
