#pragma once

class light;

#define DU_SPHERE_NUMVERTEX 92
#define DU_SPHERE_NUMFACES	180
#define DU_CONE_NUMVERTEX	18
#define DU_CONE_NUMFACES	32

class CRenderTarget		: public IRender_Target
{
private:
	u32							dwWidth;
	u32							dwHeight;
	u32							dwAccumulatorClearMark;
	u32							dwZBufferClearMark;
	u32							dwLightMarkerID;
public:
	// 
	IBlender*					b_occq;
	IBlender*					b_accum_mask;
	IBlender*					b_accum_direct;
	IBlender*					b_accum_point;
	IBlender*					b_accum_spot;
	IBlender*					b_accum_reflected;
	IBlender*					b_bloom;
	IBlender*					b_luminance;
	IBlender*					b_combine;
#ifdef DEBUG
	xr_vector<std::pair<Fsphere,Fcolor> >		dbg_spheres;
#endif

	// MRT-path
	ref_rt						rt_Depth;		// Z-buffer like - initial depth
	ref_rt						rt_Position;	// 64bit,	fat	(x,y,z,?)				(eye-space)
	ref_rt						rt_Normal;		// 64bit,	fat	(x,y,z,hemi)			(eye-space)
	ref_rt						rt_Color;		// 32bit,	fat	(r,g,b,specular-gloss)	(or decompressed MET-8-8-8-8)

	// 
	ref_rt						rt_Accumulator;	// 32bit		(r,g,b,specular)
	ref_rt						rt_Generic_0;	// 32bit		(r,g,b,a)				// post-process, intermidiate results, etc.
	ref_rt						rt_Generic_1;	// 32bit		(r,g,b,a)				// post-process, intermidiate results, etc.
	ref_rt						rt_Bloom_1;		// 32bit, dim/4	(r,g,b,?)
	ref_rt						rt_Bloom_2;		// 32bit, dim/4	(r,g,b,?)
	ref_rt						rt_LUM_64;		// 64bit, 64x64,	log-average in all components
	ref_rt						rt_LUM_8;		// 64bit, 8x8,		log-average in all components

	ref_rt						rt_LUM_pool	[4]	;	// 1xfp32,1x1,		exp-result -> scaler
	ref_texture					t_LUM_src		;	// source
	ref_texture					t_LUM_dest		;	// destination & usage for current frame

	// smap
	ref_rt						rt_smap_surf;	// 32bit,		color
	ref_rt						rt_smap_depth;	// 24(32) bit,	depth 
	IDirect3DSurface9*			rt_smap_ZB;		//

	// Textures
	IDirect3DVolumeTexture9*	t_material_surf;
	ref_texture					t_material;

	IDirect3DTexture9*			t_noise_surf	[TEX_jitter_count];
	ref_texture					t_noise			[TEX_jitter_count];
private:
	// OCCq
	ref_shader					s_occq;

	// Accum
	ref_shader					s_accum_mask	;
	ref_shader					s_accum_direct	;
	ref_shader					s_accum_point	;
	ref_shader					s_accum_spot	;
	ref_shader					s_accum_reflected;

	ref_geom					g_accum_point	;
	ref_geom					g_accum_spot	;
	ref_geom					g_accum_omnipart;

	IDirect3DVertexBuffer9*		g_accum_point_vb;
	IDirect3DIndexBuffer9*		g_accum_point_ib;

	IDirect3DVertexBuffer9*		g_accum_omnip_vb;
	IDirect3DIndexBuffer9*		g_accum_omnip_ib;

	IDirect3DVertexBuffer9*		g_accum_spot_vb	;
	IDirect3DIndexBuffer9*		g_accum_spot_ib	;

	// Bloom
	ref_geom					g_bloom_build;
	ref_geom					g_bloom_filter;
	ref_shader					s_bloom_dbg_1;
	ref_shader					s_bloom_dbg_2;
	ref_shader					s_bloom;
	float						f_bloom_factor;

	// Luminance
	ref_shader					s_luminance;
	float						f_luminance_adapt;

	// Combine
	ref_geom					g_combine;
	ref_geom					g_combine_2UV;
	ref_geom					g_aa_blur;
	ref_geom					g_aa_AA;
	ref_shader					s_combine_dbg_0;
	ref_shader					s_combine_dbg_1;
	ref_shader					s_combine_dbg_Accumulator;
	ref_shader					s_combine;
private:
	float						param_blur;
	float						param_gray;
	float						param_duality_h;
	float						param_duality_v;
	float						param_noise;
	float						param_noise_scale;
	float						param_noise_fps;
	u32							param_color_base;
	u32							param_color_gray;
	u32							param_color_add;
public:
	void						OnDeviceCreate			();
	void						OnDeviceDestroy			();
	void						accum_point_geom_create	();
	void						accum_point_geom_destroy();
	void						accum_omnip_geom_create	();
	void						accum_omnip_geom_destroy();
	void						accum_spot_geom_create	();
	void						accum_spot_geom_destroy	();

	void						u_stencil_optimize		();
	void						u_compute_texgen_screen	(Fmatrix& dest);
	void						u_compute_texgen_jitter	(Fmatrix& dest);
	void						u_setrt					(const ref_rt& _1, const ref_rt& _2, const ref_rt& _3, IDirect3DSurface9* zb);
	void						u_setrt					(u32 W, u32 H, IDirect3DSurface9* _1, IDirect3DSurface9* _2, IDirect3DSurface9* _3, IDirect3DSurface9* zb);

	void						phase_scene				();
	void						phase_occq				();
	void						phase_wallmarks			();
	void						phase_smap_direct		(light* L);
	void						phase_smap_direct_tsh	(light* L);
	void						phase_smap_spot_clear	();
	void						phase_smap_spot			(light* L);
	void						phase_smap_spot_tsh		(light* L);
	void						phase_accumulator		();
	void						phase_accumulator_init	();
	void						shadow_direct			(light* L, u32 dls_phase);
	BOOL						enable_scissor			(light* L);		// true if intersects near plane
	void						draw_volume				(light* L);
	void						accum_direct			(u32	sub_phase);
	void						accum_point				(light* L);
	void						accum_spot				(light* L);
	void						accum_reflected			(light* L);
	void						phase_bloom				();
	void						phase_luminance			();
	void						phase_distortion		();
	void						phase_combine			();

	virtual void				set_blur				(float	f)		{ param_blur=f;						}
	virtual void				set_gray				(float	f)		{ param_gray=f;						}
	virtual void				set_duality_h			(float	f)		{ param_duality_h=_abs(f);			}
	virtual void				set_duality_v			(float	f)		{ param_duality_v=_abs(f);			}
	virtual void				set_noise				(float	f)		{ param_noise=f;					}
	virtual void				set_noise_scale			(float	f)		{ param_noise_scale=f;				}
	virtual void				set_noise_fps			(float	f)		{ param_noise_fps=_abs(f)+EPS_S;	}
	virtual void				set_color_base			(u32	f)		{ param_color_base=f;				}
	virtual void				set_color_gray			(u32	f)		{ param_color_gray=f;				}
	virtual void				set_color_add			(u32	f)		{ param_color_add=f;				}
	virtual u32					get_width				()				{ return dwWidth;					}
	virtual u32					get_height				()				{ return dwHeight;					}
};
