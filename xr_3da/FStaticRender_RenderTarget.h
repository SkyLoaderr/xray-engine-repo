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
	Shader*				pShaderDuality;
	Shader*				pShaderNoise;
	CVS*				pVS;
	CVS*				pVS2;
	
	BOOL				Create				();
	
	void				e_render_noise		();
	void				e_render_duality	();

	float				im_noise_time;
	u32					im_noise_shift_w;
	u32					im_noise_shift_h;
	
	float				param_blur;
	float				param_gray;
	float				param_duality;
	float				param_noise;
	float				param_noise_scale;
	u32					param_noise_color;
	float				param_noise_fps;
public:
	CRenderTarget		();

	void				OnDeviceCreate		();
	void				OnDeviceDestroy		();

	BOOL				NeedPostProcess		()	{ return (param_blur>0.01f)||(param_gray>0.01f)||(param_noise>0.01f)||(param_duality>0.001f); }
	BOOL				Available			()	{ return bAvailable; }
	BOOL				Perform				();

	void				Begin				();
	void				End					();

	virtual void		eff_load			(LPCSTR n);

	virtual void		set_blur			(float f)		{ param_blur=f;			}
	virtual void		set_gray			(float f)		{ param_gray=f;			}
	virtual void		set_duality			(float f)		{ param_duality=_abs(f);}
	virtual void		set_noise			(float f)		{ param_noise=f;		}
	virtual void		set_noise_scale		(float f)		{ param_noise_scale=f;	}
	virtual void		set_noise_color		(u32 f)			{ param_noise_color=f;	}
	virtual void		set_noise_fps		(float f)		{ param_noise_fps=_abs(f)+EPS_S;	}

	virtual u32			get_width			()				{ return curWidth;	}
	virtual u32			get_height			()				{ return curHeight;	}
};
