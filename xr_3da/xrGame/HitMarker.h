#ifndef __XR_HITMARKER_H__
#define __XR_HITMARKER_H__
#pragma once

class CHitMarker	: public pureDeviceDestroy, public pureDeviceCreate
{
public:
	float					fHitMarks[4];
	Shader*					hShader;
	CVS*					hVS;
public:
							CHitMarker	();
							~CHitMarker	();

	void					Render		();
	void					Hit			(int id);

	virtual void			OnDeviceDestroy	();
	virtual void			OnDeviceCreate	();
};

#endif // __XR_HITMARKER_H__
