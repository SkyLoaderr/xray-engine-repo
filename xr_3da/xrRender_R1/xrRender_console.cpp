#include "stdafx.h"
#pragma hdrstop

#include "xrRender_console.h"

// Common
int			ps_r__Supersample			= 1;
int			ps_r__Anisotropic			= 4;
int			ps_r__LightSleepFrames		= 10;

float		ps_r__Detail_w_rot1			= 30.f;
float		ps_r__Detail_w_rot2			= 1.f;
float		ps_r__Detail_w_speed		= 2.f;
float		ps_r__Detail_l_ambient		= 0.9f;
float		ps_r__Detail_l_aniso		= 0.25f;
float		ps_r__Detail_density		= 0.15f;
float		ps_r__Detail_rainbow_hemi	= 0.75f;

float		ps_r__Tree_w_rot			= 10.0f;
float		ps_r__Tree_w_speed			= 1.00f;
float		ps_r__Tree_w_amp			= 0.01f;
Fvector		ps_r__Tree_Wave				= {.1f, .01f, .11f};

// R1
float		ps_r1_ssaDISCARD			= 4.f;
float		ps_r1_ssaDONTSORT			= 32.f;
float		ps_r1_ssaLOD_A				= 64.f;
float		ps_r1_ssaLOD_B				= 48.f;
float		ps_r1_ssaHZBvsTEX			= 256.f;

// R1-specific
int			ps_r1_GlowsPerFrame			= 16;	// r1-only
int			ps_r1_SH_Blur				= 0;	// r1-only

// R2
float		ps_r2_ssaDISCARD			= 4.f;
float		ps_r2_ssaDONTSORT			= 32.f;
float		ps_r2_ssaLOD_A				= 64.f;
float		ps_r2_ssaLOD_B				= 48.f;
float		ps_r2_ssaHZBvsTEX			= 256.f;

// R2-specific
Flags32		ps_r2_ls_flags				= { R2FLAG_SUN | R2FLAG_SKY | R2FLAG_FASTBLOOM | R2FLAG_AA };	// r2-only
float		ps_r2_df_parallaxh			= 0.02f;
float		ps_r2_tonemap_middlegray	= 0.18f;// r2-only
float		ps_r2_ls_dynamic_range		= 2.f;	// r2-only
float		ps_r2_ls_bloom_kernel_g		= 3.3f;	// r2-only
float		ps_r2_ls_bloom_kernel_b		= .6f;	// r2-only
float		ps_r2_ls_bloom_speed		= 10.f;	// r2-only
float		ps_r2_ls_dsm_kernel			= .7f;	// r2-only
float		ps_r2_ls_psm_kernel			= .7f;	// r2-only
float		ps_r2_ls_ssm_kernel			= .7f;	// r2-only
float		ps_r2_ls_bloom_threshold	= .3f;	// r2-only
Fvector		ps_r2_aa_barier				= { .8f, .1f, 0};	// r2-only
Fvector		ps_r2_aa_weight				= { .25f,.25f,0};	// r2-only
float		ps_r2_aa_kernel				= .5f;				// r2-only
int			ps_r2_GI_depth				= 1;				// 1..5
int			ps_r2_GI_photons			= 16;				// 8..64
float		ps_r2_GI_clip				= EPS_L;			// EPS
float		ps_r2_GI_refl				= .9f;				// .9f
float		ps_r2_emap_height			= 0.05f;			// 0.05f
float		ps_r2_emap_range			= 30.f;				// 0.05f
int			ps_r2_emap_slices			= 4;				// 4

#ifndef _EDITOR
#include	"..\xr_ioconsole.h"
#include	"..\xr_ioc_cmd.h"

//-----------------------------------------------------------------------
class CCC_Aniso		: public CCC_Integer
{
public:
	CCC_Aniso(LPCSTR N, int*	v) : CCC_Integer(N, v, 2, 16) { *v = 4; };
	virtual void Execute(LPCSTR args)
	{
		CCC_Integer::Execute	(args);
		for (u32 i=0; i<HW.Caps.raster.dwStages; i++)
			CHK_DX(HW.pDevice->SetSamplerState( i, D3DSAMP_MAXANISOTROPY, *value	));
	}
};
//-----------------------------------------------------------------------
void		xrRender_initconsole	()
{
	// Common
	CMD4(CCC_Integer,	"r__supersample",		&ps_r__Supersample,			1,		4		);
	CMD2(CCC_Aniso,		"r__aniso_tf",			&ps_r__Anisotropic);
	CMD4(CCC_Integer,	"r__lsleep_frames",		&ps_r__LightSleepFrames,	4,		30		);
	CMD4(CCC_Float,		"r__detail_w_rot1",		&ps_r__Detail_w_rot1,		1.f,	180.f	);
	CMD4(CCC_Float,		"r__detail_w_rot2",		&ps_r__Detail_w_rot2,		1.f,	180.f	);
	CMD4(CCC_Float,		"r__detail_w_speed",	&ps_r__Detail_w_speed,		1.f,	4.f		);
	CMD4(CCC_Float,		"r__detail_l_ambient",	&ps_r__Detail_l_ambient,	.5f,	.95f	);
	CMD4(CCC_Float,		"r__detail_l_aniso",	&ps_r__Detail_l_aniso,		.1f,	.5f		);
	CMD4(CCC_Float,		"r__detail_density",	&ps_r__Detail_density,		.05f,	0.3f	);
	CMD4(CCC_Float,		"r__detail_rainbow_h",	&ps_r__Detail_rainbow_hemi,	.00f,	1.0f	);

	Fvector	tw_min,tw_max;
	tw_min.set			(EPS,EPS,EPS);
	tw_max.set			(2,2,2);
	CMD4(CCC_Float,		"r__d_tree_w_rot",		&ps_r__Tree_w_rot,			.01f, 100.f		);
	CMD4(CCC_Float,		"r__d_tree_w_speed",	&ps_r__Tree_w_speed,		1.0f, 4.f		);
	CMD4(CCC_Float,		"r__d_tree_w_amp",		&ps_r__Tree_w_amp,			.001f, 1.f		);
	CMD4(CCC_Vector3,	"r__d_tree_wave",		&ps_r__Tree_Wave,			tw_min, tw_max	);

	// R1
	CMD4(CCC_Float,		"r1_ssa_lod_a",			&ps_r1_ssaLOD_A,			16,		96		);
	CMD4(CCC_Float,		"r1_ssa_lod_b",			&ps_r1_ssaLOD_B,			16,		64		);
	CMD4(CCC_Float,		"r1_ssa_discard",		&ps_r1_ssaDISCARD,			1,		16		);
	CMD4(CCC_Float,		"r1_ssa_dontsort",		&ps_r1_ssaDONTSORT,			16,		128		);
	CMD4(CCC_Float,		"r1_ssa_hzb_vs_tex",	&ps_r1_ssaHZBvsTEX,			16,		512		);

	// R1-specific
	CMD4(CCC_Integer,	"r1_glows_per_frame",	&ps_r1_GlowsPerFrame,		2,		32		);
	CMD4(CCC_Integer,	"r1_sh_blur",			&ps_r1_SH_Blur,				0,		2		);

	// R2
	CMD4(CCC_Float,		"r2_ssa_lod_a",			&ps_r2_ssaLOD_A,			16,		96		);
	CMD4(CCC_Float,		"r2_ssa_lod_b",			&ps_r2_ssaLOD_B,			32,		64		);
	CMD4(CCC_Float,		"r2_ssa_discard",		&ps_r2_ssaDISCARD,			1,		16		);
	CMD4(CCC_Float,		"r2_ssa_dontsort",		&ps_r2_ssaDONTSORT,			16,		128		);
	CMD4(CCC_Float,		"r2_ssa_hzb_vs_tex",	&ps_r2_ssaHZBvsTEX,			16,		512		);

	// R2-specific
	CMD4(CCC_Float,		"r2_tonemap_middlegray",&ps_r2_tonemap_middlegray,	0.0f,	2.0f	);
	CMD4(CCC_Float,		"r2_ls_dynamic_range",	&ps_r2_ls_dynamic_range,	.5f,	8.f		);
	CMD4(CCC_Float,		"r2_ls_bloom_kernel_g",	&ps_r2_ls_bloom_kernel_g,	1.f,	7.f		);
	CMD4(CCC_Float,		"r2_ls_bloom_kernel_b",	&ps_r2_ls_bloom_kernel_b,	0.01f,	1.f		);
	CMD4(CCC_Float,		"r2_ls_bloom_threshold",&ps_r2_ls_bloom_threshold,	0.f,	1.f		);
	CMD4(CCC_Float,		"r2_ls_bloom_speed",	&ps_r2_ls_bloom_speed,		0.f,	100.f	);
	CMD3(CCC_Mask,		"r2_ls_bloom_fast",		&ps_r2_ls_flags,			R2FLAG_FASTBLOOM);
	CMD4(CCC_Float,		"r2_ls_dsm_kernel",		&ps_r2_ls_dsm_kernel,		.1f,	3.f		);
	CMD4(CCC_Float,		"r2_ls_psm_kernel",		&ps_r2_ls_psm_kernel,		.1f,	3.f		);
	CMD4(CCC_Float,		"r2_ls_ssm_kernel",		&ps_r2_ls_ssm_kernel,		.1f,	3.f		);
	CMD3(CCC_Mask,		"r2_sun",				&ps_r2_ls_flags,			R2FLAG_SUN		);
	CMD3(CCC_Mask,		"r2_sky",				&ps_r2_ls_flags,			R2FLAG_SKY		);

	CMD3(CCC_Mask,		"r2_aa",				&ps_r2_ls_flags,			R2FLAG_AA);
	CMD4(CCC_Float,		"r2_aa_kernel",			&ps_r2_aa_kernel,			0.3f,	0.7f	);

	CMD3(CCC_Mask,		"r2_gi",				&ps_r2_ls_flags,			R2FLAG_GI);
	CMD4(CCC_Float,		"r2_gi_clip",			&ps_r2_GI_clip,				EPS_L,	0.1f	);
	CMD4(CCC_Integer,	"r2_gi_depth",			&ps_r2_GI_depth,			1,		5		);
	CMD4(CCC_Integer,	"r2_gi_photons",		&ps_r2_GI_photons,			8,		256		);
	CMD4(CCC_Float,		"r2_gi_refl",			&ps_r2_GI_refl,				EPS_L,	0.99f	);

	CMD4(CCC_Integer,	"r2_emap_slices",		&ps_r2_emap_slices,			2,		256		);
	CMD4(CCC_Float,		"r2_emap_height",		&ps_r2_emap_height,			0.01f,	0.1f	);
	CMD4(CCC_Float,		"r2_emap_range",		&ps_r2_emap_range,			10.f,	100.f	);

	CMD3(CCC_Mask,		"r2_parallax",			&ps_r2_ls_flags,			R2FLAG_PARALLAX);
	CMD4(CCC_Float,		"r2_parallax_h",		&ps_r2_df_parallaxh,		.0f,	.5f		);

	tw_min.set			(0,0,0);	tw_max.set			(1,1,1);
	CMD4(CCC_Vector3,	"r2_aa_break",			&ps_r2_aa_barier,			tw_min, tw_max	);

	tw_min.set			(0,0,0);	tw_max.set			(1,1,1);
	CMD4(CCC_Vector3,	"r2_aa_weight",			&ps_r2_aa_weight,			tw_min, tw_max	);
}
#endif
