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
	CBlender*					b_accum_mask;
	CBlender*					b_accum_direct;
	CBlender*					b_accum_point_s;
	CBlender*					b_accum_point_uns;
	CBlender*					b_accum_spot_s;
	CBlender*					b_accum_spot_uns;
	CBlender*					b_bloom;
	CBlender*					b_combine;

	// MRT-path
	CRT*						rt_Position;	// 64bit, fat	(x,y,z,?)				(eye-space)
	CRT*						rt_Normal;		// 64bit, fat	(x,y,z,?)				(eye-space)
	CRT*						rt_Color;		// 64bit, fat	(r,g,b,specular-gloss)

	// MET-path
	CRT*						rt_Deffer;		// NVE3, 8*4_8*4_16*2_16*2 = 128bit

	CRT*						rt_Accumulator;	// 32bit		(r,g,b,specular)
	CRT*						rt_Generic;		// 32bit		(r,g,b,a)				// post-process, intermidiate results, etc.
	CRT*						rt_Bloom_1;		// 32bit, dim/4	(r,g,b,?)
	CRT*						rt_Bloom_2;		// 32bit, dim/4	(r,g,b,?)
	IDirect3DSurface9*			rt_Bloom_ZB;	// $$$ dumb ZB for nv3xx

	// D-smap
	CRT*						rt_smap_d_surf;	// 32bit, depth 
	CRT*						rt_smap_d_depth;// 24(32) bit, depth 
	IDirect3DSurface9*			rt_smap_d_ZB;	//

	// P-smap
	CRTC*						rt_smap_p;
	IDirect3DSurface9*			rt_smap_p_ZB;

	// Textures
	IDirect3DTexture9*			t_material_surf;
	CTexture*					t_material;
	IDirect3DTexture9*			t_ds2fade_surf;
	CTexture*					t_ds2fade;
private:
	// Accum
	Shader*						s_accum_mask;
	Shader*						s_accum_direct;
	Shader*						s_accum_point_s;
	Shader*						s_accum_point_uns;
	Shader*						s_accum_spot_s;
	Shader*						s_accum_spot_uns;
	SGeometry*					g_accum_point;
	SGeometry*					g_accum_point_tl;
	IDirect3DVertexBuffer9*		g_accum_point_vb;
	IDirect3DIndexBuffer9*		g_accum_point_ib;

	// Bloom
	SGeometry*					g_bloom_build;
	SGeometry*					g_bloom_filter;
	Shader*						s_bloom_dbg_1;
	Shader*						s_bloom_dbg_2;
	Shader*						s_bloom;

	// Combine
	SGeometry*					g_combine;
	Shader*						s_combine_dbg_DepthD;
	Shader*						s_combine_dbg_Normal;
	Shader*						s_combine_dbg_Accumulator;
	Shader*						s_combine;
private:
	float						param_blur;
	float						param_gray;
	float						param_duality_h;
	float						param_duality_v;
	float						param_noise;
	float						param_noise_scale;
	u32							param_noise_color;
	float						param_noise_fps;
public:
	void						OnDeviceCreate			();
	void						OnDeviceDestroy			();
	void						accum_point_geom_create	();
	void						accum_point_geom_destroy();

	void						u_setrt					(CRT* _1, CRT* _2, CRT* _3, IDirect3DSurface9* zb);
	void						u_setrt					(IDirect3DSurface9* _1, IDirect3DSurface9* _2, IDirect3DSurface9* _3, IDirect3DSurface9* zb);

	void						phase_scene				();
	void						phase_scene_sky			();
	void						phase_smap_direct		();
	void						phase_smap_point		(u32 pls_phase);
	void						phase_smap_spot			();
	void						phase_accumulator		();
	void						phase_accumulator_init	();
	void						shadow_direct			(u32 dls_phase);
	void						accum_direct			();
	void						accum_point_shadow		(light* L);
	void						accum_point_unshadow	(light* L);
	void						accum_spot_shadow		(light* L);
	void						accum_spot_unshadow		(light* L);
	void						phase_bloom				();
	void						phase_combine			();

	virtual void				eff_load				(LPCSTR n)		{};

	virtual void				set_blur				(float f)		{ param_blur=f;						}
	virtual void				set_gray				(float f)		{ param_gray=f;						}
	virtual void				set_duality_h			(float f)		{ param_duality_h=_abs(f);			}
	virtual void				set_duality_v			(float f)		{ param_duality_v=_abs(f);			}
	virtual void				set_noise				(float f)		{ param_noise=f;					}
	virtual void				set_noise_scale			(float f)		{ param_noise_scale=f;				}
	virtual void				set_noise_color			(u32 f)			{ param_noise_color=f;				}
	virtual void				set_noise_fps			(float f)		{ param_noise_fps=_abs(f)+EPS_S;	}

	virtual u32					get_width				()				{ return dwWidth;			}
	virtual u32					get_height				()				{ return dwHeight;			}
};
