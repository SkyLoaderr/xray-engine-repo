#include "stdafx.h"
#pragma hdrstop

#include "xrRender_console.h"

// Common
int			ps_r__Supersample		= 0;

float		ps_r__Detail_w_rot1		= 30.f;
float		ps_r__Detail_w_rot2		= 1.f;
float		ps_r__Detail_w_speed	= 2.f;
float		ps_r__Detail_l_ambient	= 0.9f;
float		ps_r__Detail_l_aniso	= 0.25f;
float		ps_r__Detail_density	= 0.15f;

float		ps_r__Tree_w_rot		= 10.0f;
float		ps_r__Tree_w_speed		= 1.00f;
float		ps_r__Tree_w_amp		= 0.01f;
Fvector		ps_r__Tree_Wave			= {.1f, .01f, .11f};

// R1
float		ps_r1_ssaDISCARD		= 4.f;
float		ps_r1_ssaDONTSORT		= 32.f;
float		ps_r1_ssaLOD_A			= 64.f;
float		ps_r1_ssaLOD_B			= 48.f;
float		ps_r1_ssaHZBvsTEX		= 256.f;

// R1-specific
int			ps_r1_GlowsPerFrame		= 16;	// r1-only
int			ps_r1_SH_Blur			= 0;	// r1-only

// R2
float		ps_r2_ssaDISCARD		= 4.f;
float		ps_r2_ssaDONTSORT		= 32.f;
float		ps_r2_ssaLOD_A			= 64.f;
float		ps_r2_ssaLOD_B			= 48.f;
float		ps_r2_ssaHZBvsTEX		= 256.f;

// R2-specific
float		ps_r2_ls_dynamic_range	= 2.f;	// r2-only
float		ps_r2_ls_bloom_kernel	= 3.3f;	// r2-only
float		ps_r2_ls_dsm_kernel		= .7f;	// r2-only
float		ps_r2_ls_psm_kernel		= .7f;	// r2-only
float		ps_r2_ls_ssm_kernel		= .7f;	// r2-only
float		ps_r2_ls_bloom_threshold= .3f;	// r2-only
float		ps_r2_ls_spower			= 64.f;
Flags32		ps_r2_ls_flags			= { R2FLAG_SUN | R2FLAG_SKY };	// r2-only

#ifndef _EDITOR
#include	"..\xr_ioconsole.h"
#include	"..\xr_ioc_cmd.h"
void		xrRender_initconsole	()
{
	// Common
	CMD4(CCC_Integer,	"r__supersample",		&ps_r__Supersample,			0,		4		);
	CMD4(CCC_Float,		"r__detail_w_rot1",		&ps_r__Detail_w_rot1,		1.f,	180.f	);
	CMD4(CCC_Float,		"r__detail_w_rot2",		&ps_r__Detail_w_rot2,		1.f,	180.f	);
	CMD4(CCC_Float,		"r__detail_w_speed",	&ps_r__Detail_w_speed,		1.f,	4.f		);
	CMD4(CCC_Float,		"r__detail_l_ambient",	&ps_r__Detail_l_ambient,	.5f,	.95f	);
	CMD4(CCC_Float,		"r__detail_l_aniso",	&ps_r__Detail_l_aniso,		.1f,	.5f		);
	CMD4(CCC_Float,		"r__detail_density",	&ps_r__Detail_density,		.05f,	0.3f	);

	Fvector	tw_min,tw_max;
	tw_min.set			(EPS,EPS,EPS);
	tw_max.set			(2,2,2);
	CMD4(CCC_Float,		"r__d_tree_w_rot",		&ps_r__Tree_w_rot,			.01f, 100.f		);
	CMD4(CCC_Float,		"r__d_tree_w_speed",	&ps_r__Tree_w_speed,		1.0f, 4.f		);
	CMD4(CCC_Float,		"r__d_tree_w_amp",		&ps_r__Tree_w_amp,			.001f, 1.f		);
	CMD4(CCC_Vector3,	"r__d_tree_wave",		&ps_r__Tree_Wave,			tw_min, tw_max	);

	// R1
	CMD4(CCC_Float,		"r1_ssa_lod_a",			&ps_r1_ssaLOD_A,			16,		96		);
	CMD4(CCC_Float,		"r1_ssa_lod_b",			&ps_r1_ssaLOD_B,			32,		64		);
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
	CMD4(CCC_Float,		"r2_ls_dynamic_range",	&ps_r2_ls_dynamic_range,	.5f,	8.f		);
	CMD4(CCC_Float,		"r2_ls_bloom_kernel",	&ps_r2_ls_bloom_kernel,		1.f,	7.f		);
	CMD4(CCC_Float,		"r2_ls_bloom_threshold",&ps_r2_ls_bloom_threshold,	0.f,	1.f		);
	CMD4(CCC_Float,		"r2_ls_dsm_kernel",		&ps_r2_ls_dsm_kernel,		.1f,	3.f		);
	CMD4(CCC_Float,		"r2_ls_psm_kernel",		&ps_r2_ls_psm_kernel,		.1f,	3.f		);
	CMD4(CCC_Float,		"r2_ls_ssm_kernel",		&ps_r2_ls_ssm_kernel,		.1f,	3.f		);
	CMD4(CCC_Float,		"r2_ls_spower",			&ps_r2_ls_spower,			1.f,	512.f	);
	CMD3(CCC_Mask,		"r2_sun",				&ps_r2_ls_flags,			R2FLAG_SUN);
	CMD3(CCC_Mask,		"r2_sky",				&ps_r2_ls_flags,			R2FLAG_SKY);
	CMD3(CCC_Mask,		"r2_bump_af",			&ps_r2_ls_flags,			R2FLAG_BUMP_AF);
}
#endif
