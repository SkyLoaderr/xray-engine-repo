#ifndef __XR_HUDCURSOR_H__
#define __XR_HUDCURSOR_H__
#pragma once

class CHUDCursor : public pureDeviceDestroy, public pureDeviceCreate
{
	Shader*					hShader;
	CVS*					hVS;	
public:
							CHUDCursor	();
							~CHUDCursor	();
	void					Render		();

	virtual void			OnDeviceDestroy	();
	virtual void			OnDeviceCreate	();
};

#endif //__XR_HUDCURSOR_H__
