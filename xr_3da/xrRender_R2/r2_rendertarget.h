#pragma once

class CRenderTarget		: public IRender_Target
{
private:
	CRT*				rt_Position;	// 64bit, fat	(x,y,z,?)				(eye-space)
	CRT*				rt_Normal;		// 64bit, fat	(x,y,z,?)				(eye-space)
	CRT*				rt_Color;		// 64bit, fat	(r,g,b,specular-gloss)
	CRT*				rt_Accumulator;	// 32bit		(r,g,b,specular)
	CRT*				rt_Bloom_1;		// 32bit, dim/4	(r,g,b,?)
	CRT*				rt_Bloom_2;		// 32bit, dim/4	(r,g,b,?)
private:
	SGeometry*			g_combine;
	Shader*				s_combine_dbg_Color;
	Shader*				s_combine_dbg_Normal;
	Shader*				s_combine_dbg_Accumulator;
private:
	float				param_blur;
	float				param_gray;
	float				param_duality_h;
	float				param_duality_v;
	float				param_noise;
	float				param_noise_scale;
	u32					param_noise_color;
	float				param_noise_fps;
public:
	void				OnDeviceCreate		();
	void				OnDeviceDestroy		();

	void				phase_scene			();
	void				phase_combine		();

	virtual void		eff_load			(LPCSTR n)		{};

	virtual void		set_blur			(float f)		{ param_blur=f;						}
	virtual void		set_gray			(float f)		{ param_gray=f;						}
	virtual void		set_duality_h		(float f)		{ param_duality_h=_abs(f);			}
	virtual void		set_duality_v		(float f)		{ param_duality_v=_abs(f);			}
	virtual void		set_noise			(float f)		{ param_noise=f;					}
	virtual void		set_noise_scale		(float f)		{ param_noise_scale=f;				}
	virtual void		set_noise_color		(u32 f)			{ param_noise_color=f;				}
	virtual void		set_noise_fps		(float f)		{ param_noise_fps=_abs(f)+EPS_S;	}

	virtual u32			get_width			()				{ return Device.dwWidth;			}
	virtual u32			get_height			()				{ return Device.dwHeight;			}
};
