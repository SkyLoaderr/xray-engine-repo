#pragma once

class light;

#define DU_SPHERE_NUMVERTEX 92
#define DU_SPHERE_NUMFACES	180

class CRenderTarget		: public IRender_Target
{
private:
	u32							dwWidth;
	u32							dwHeight;
	u32							dwAccumulatorClearMark;
	u32							dwLightMarkerID;
private:
	// 
	CBlender*					b_accum_direct_mask;
	CBlender*					b_accum_direct;
	CBlender*					b_accum_point;
	CBlender*					b_combine;

	//
	CRT*						rt_Position;	// 64bit, fat	(x,y,z,?)				(eye-space)
	CRT*						rt_Normal;		// 64bit, fat	(x,y,z,?)				(eye-space)
	CRT*						rt_Color;		// 64bit, fat	(r,g,b,specular-gloss)
	CRT*						rt_Accumulator;	// 32bit		(r,g,b,specular)
	CRT*						rt_Bloom_1;		// 32bit, dim/4	(r,g,b,?)
	CRT*						rt_Bloom_2;		// 32bit, dim/4	(r,g,b,?)

	// D-smap
	CRT*						rt_smap_d;		// 32bit, (depth)	(eye-space)
	IDirect3DSurface9*			rt_smap_d_ZB;

	// P-smap
	CRTC*						rt_smap_p;
	IDirect3DSurface9*			rt_smap_p_ZB;
private:
	// 
	Shader*						s_accum_direct_mask;
	Shader*						s_accum_direct;
	Shader*						s_accum_point;
	SGeometry*					g_accum_point;
	IDirect3DVertexBuffer9*		g_accum_point_vb;
	IDirect3DIndexBuffer9*		g_accum_point_ib;

	//
	SGeometry*					g_combine;
	Shader*						s_combine_dbg_Position;
	Shader*						s_combine_dbg_Normal;
	Shader*						s_combine_dbg_Color;
	Shader*						s_combine_dbg_Accumulator;
	Shader*						s_combine;
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
	void				OnDeviceCreate			();
	void				OnDeviceDestroy			();
	void				accum_point_geom_create	();
	void				accum_point_geom_destroy();

	void				phase_scene				();
	void				phase_smap_direct		();
	void				phase_smap_point		(u32 pls_phase);
	void				phase_accumulator		();
	void				shadow_direct			(u32 dls_phase);
	void				accum_direct			();
	void				accum_point_shadow		(light* L);
	void				accum_point_unshadow	(light* L);
	void				phase_combine			();

	virtual void		eff_load				(LPCSTR n)		{};

	virtual void		set_blur				(float f)		{ param_blur=f;						}
	virtual void		set_gray				(float f)		{ param_gray=f;						}
	virtual void		set_duality_h			(float f)		{ param_duality_h=_abs(f);			}
	virtual void		set_duality_v			(float f)		{ param_duality_v=_abs(f);			}
	virtual void		set_noise				(float f)		{ param_noise=f;					}
	virtual void		set_noise_scale			(float f)		{ param_noise_scale=f;				}
	virtual void		set_noise_color			(u32 f)			{ param_noise_color=f;				}
	virtual void		set_noise_fps			(float f)		{ param_noise_fps=_abs(f)+EPS_S;	}

	virtual u32			get_width				()				{ return dwWidth;			}
	virtual u32			get_height				()				{ return dwHeight;			}
};
