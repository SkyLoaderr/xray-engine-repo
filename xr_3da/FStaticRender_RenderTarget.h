#pragma once

#include "render.h"

class CRenderTarget		: public CRender_target
{
private:
	BOOL				bAvailable;
	u32					rtWidth;
	u32					rtHeight;

	u32					curWidth;
	u32					curHeight;

	CRT*				RT;
	IDirect3DSurface8*	ZB;

	Shader*				pShaderSet;
	Shader*				pShaderGray;
	Shader*				pShaderBlend;
	Shader*				pShaderNoise;
	CVS*				pVS;
	
	BOOL				Create				();
	void				e_render_noise		();
	
	float				param_blur;
	float				param_gray;
	float				param_noise;
public:
	CRenderTarget		();

	void				OnDeviceCreate		();
	void				OnDeviceDestroy		();

	BOOL				NeedPostProcess		()	{ return (param_blur>0.01f)||(param_gray>0.01f)||(param_noise>0.01f); }
	BOOL				Available			()	{ return bAvailable; }
	BOOL				Perform				();

	void				Begin				();
	void				End					();

	virtual void		set_blur			(float f)		{ param_blur=f;		}
	virtual void		set_gray			(float f)		{ param_gray=f;		}
	virtual void		set_noise			(float f)		{ param_noise=f;	}

	virtual u32			get_width			()				{ return curWidth;	}
	virtual u32			get_height			()				{ return curHeight;	}
};
