#include "stdafx.h"
#include "..\xr_ioconsole.h"
#include "..\xr_ioc_cmd.h"

extern void			xrRender_initconsole	()
{
	// Common
	CMD4(CCC_Integer,	"r__supersample",		&ps_r__Supersample,			0,		4		);
	CMD4(CCC_Float,		"r__detail_w_rot1",		&ps_r__Detail_w_rot1,		1.f,	180.f	);
	CMD4(CCC_Float,		"r__detail_w_rot2",		&ps_r__Detail_w_rot2,		1.f,	180.f	);
	CMD4(CCC_Float,		"r__detail_w_speed",	&ps_r__Detail_w_speed,		1.f,	4.f		);
	CMD4(CCC_Float,		"r__detail_l_ambient",	&ps_r__Detail_l_ambient,	.5f,	.95f	);
	CMD4(CCC_Float,		"r__detail_l_aniso",	&ps_r__Detail_l_aniso,		.1f,	.5f		);
	CMD4(CCC_Float,		"r__detail_density",	&ps_r__Detail_density,		.05f,	0.3f	);

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
}
