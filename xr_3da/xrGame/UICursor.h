#ifndef __XR_UICURSOR_H__
#define __XR_UICURSOR_H__
#pragma once

class CUICursor : public pureDeviceDestroy, pureDeviceCreate
{
	Shader*			hShader;
	CVertexStream*	Stream;	
public:
	Fvector2		vPos;
public:
					CUICursor	();
					~CUICursor	();
	void			Render		();
	const Fvector2&	GetPos		(){return vPos;}

	virtual void			OnDeviceDestroy	();
	virtual void			OnDeviceCreate	();
};

#endif //__XR_UICURSOR_H__
