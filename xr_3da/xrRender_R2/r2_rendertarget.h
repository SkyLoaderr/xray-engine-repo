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
	u32							dwLightMarkerID;
public:
	// 
	IBlender*					b_accum_mask;
	IBlender*					b_accum_direct;
	IBlender*					b_accum_point_s;
	IBlender*					b_accum_point_uns;
	IBlender*					b_accum_spot_s;
	IBlender*					b_accum_spot_uns;
	IBlender*					b_bloom;
	IBlender*					b_combine;
	IBlender*					b_decompress;

	// MRT-path (or decompressed MET)
	CRT*						rt_Position;	// 64bit,	fat	(x,y,z,?)				(eye-space)
	CRT*						rt_Normal;		// 64bit,	fat	(x,y,z,hemi)			(eye-space)
	CRT*						rt_Color;		// 64/32bit,fat	(r,g,b,specular-gloss)	(or decompressed MET-8-8-8-8)

	// MET-path
	CRT*						rt_Deffer;		// NVE3, 8*4_8*4_16*2_16*2 = 128bit

	// 
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
	// Decompress
	Shader*						s_decompress;	// 0=pos,1=norm,2=color

	// Accum
	Shader*						s_accum_mask;
	Shader*						s_accum_direct;
	Shader*						s_accum_point_s;
	Shader*						s_accum_point_uns;
	Shader*						s_accum_spot_s;
	Shader*						s_accum_spot_uns;
	ref_geom					g_accum_point;
	ref_geom					g_accum_spot;
	ref_geom					g_accum_point_tl;
	IDirect3DVertexBuffer9*		g_accum_point_vb;
	IDirect3DIndexBuffer9*		g_accum_point_ib;
	IDirect3DVertexBuffer9*		g_accum_spot_vb;
	IDirect3DIndexBuffer9*		g_accum_spot_ib;

	// Bloom
	ref_geom					g_bloom_build;
	ref_geom					g_bloom_filter;
	Shader*						s_bloom_dbg_1;
	Shader*						s_bloom_dbg_2;
	Shader*						s_bloom;

	// Combine
	ref_geom					g_combine;
	ref_geom					g_aa_blur;
	ref_geom					g_aa_AA;
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
	void						accum_spot_geom_create	();
	void						accum_spot_geom_destroy	();

	void						u_setrt					(CRT* _1, CRT* _2, CRT* _3, IDirect3DSurface9* zb);
	void						u_setrt					(u32 W, u32 H, IDirect3DSurface9* _1, IDirect3DSurface9* _2, IDirect3DSurface9* _3, IDirect3DSurface9* zb);

	void						phase_scene				();
	void						phase_decompress		();
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
