#ifndef xrRender_consoleH
#define xrRender_consoleH
#pragma once

// Common
extern int			ps_r__Supersample;
extern int			ps_r__Anisotropic;
extern int			ps_r__LightSleepFrames;

extern float		ps_r__Detail_w_rot1;
extern float		ps_r__Detail_w_rot2;
extern float		ps_r__Detail_w_speed;
extern float		ps_r__Detail_l_ambient;
extern float		ps_r__Detail_l_aniso;
extern float		ps_r__Detail_density;
extern float		ps_r__Detail_rainbow_hemi;

extern float		ps_r__Tree_w_rot;
extern float		ps_r__Tree_w_speed;
extern float		ps_r__Tree_w_amp;
extern Fvector		ps_r__Tree_Wave;

// R1
extern float		ps_r1_ssaDISCARD;
extern float		ps_r1_ssaDONTSORT;
extern float		ps_r1_ssaLOD_A;
extern float		ps_r1_ssaLOD_B;
extern float		ps_r1_ssaHZBvsTEX;

// R1-specific
extern int			ps_r1_GlowsPerFrame;	// r1-only
extern int			ps_r1_SH_Blur;			// r1-only

// R2
extern float		ps_r2_ssaDISCARD;
extern float		ps_r2_ssaDONTSORT;
extern float		ps_r2_ssaLOD_A;
extern float		ps_r2_ssaLOD_B;
extern float		ps_r2_ssaHZBvsTEX;

// R2-specific
extern Flags32		ps_r2_ls_flags;				// r2-only
extern float		ps_r2_df_parallaxh;			// r2-only
extern float		ps_r2_ls_dynamic_range;		// r2-only
extern float		ps_r2_ls_bloom_kernel;		// r2-only
extern float		ps_r2_ls_bloom_threshold;	// r2-only
extern float		ps_r2_ls_dsm_kernel;		// r2-only
extern float		ps_r2_ls_psm_kernel;		// r2-only
extern float		ps_r2_ls_ssm_kernel;		// r2-only
extern float		ps_r2_ls_spower;			// r2-only
extern Fvector		ps_r2_aa_barier;			// r2-only
extern Fvector		ps_r2_aa_weight;			// r2-only
extern float		ps_r2_aa_kernel;			// r2-only
extern int			ps_r2_GI_depth;				// 1..5
extern int			ps_r2_GI_photons;			// 8..256
extern float		ps_r2_GI_clip;				// EPS
extern float		ps_r2_GI_refl;				// .9f

enum
{
	R2FLAG_SUN			= (1<<0),
	R2FLAG_SKY			= (1<<1),
	R2FLAG_BUMP_AF		= (1<<2),
	R2FLAG_AA			= (1<<3),
	R2FLAG_PARALLAX		= (1<<4),
	R2FLAG_GI			= (1<<5),
	R2FLAG_FASTBLOOM	= (1<<6),
};

extern void			xrRender_initconsole	();
#endif
