#include "stdafx.h"
#pragma hdrstop

#include "xrRender_console.h"
 
// Common
//int			ps_r__Supersample			= 1;
int			ps_r__LightSleepFrames		= 10	;

float		ps_r__Detail_l_ambient		= 0.9f	;
float		ps_r__Detail_l_aniso		= 0.25f	;
float		ps_r__Detail_density		= 0.3f	;
float		ps_r__Detail_rainbow_hemi	= 0.75f	;

float		ps_r__Tree_w_rot			= 10.0f	;
float		ps_r__Tree_w_speed			= 1.00f	;
float		ps_r__Tree_w_amp			= 0.01f	;
Fvector		ps_r__Tree_Wave				= {.1f, .01f, .11f};

float		ps_r__WallmarkTTL			= 90.f	;

float		ps_r__GLOD_ssa_start		= 256.f	;
float		ps_r__GLOD_ssa_end			=  64.f	;
float		ps_r__LOD					=  1.f	;
float		ps_r__LOD_Power				=  1.5f	;

// R1
float		ps_r1_ssaDISCARD			= 4.f	;
float		ps_r1_ssaDONTSORT			= 32.f	;
float		ps_r1_ssaLOD_A				= 64.f	;
float		ps_r1_ssaLOD_B				= 48.f	;
float		ps_r1_ssaHZBvsTEX			= 256.f	;
int			ps_r1_tf_Anisotropic		= 0		;
float		ps_r1_tf_Mipbias			= -0.5f	;
Flags32		ps_r1_flags					= { R1FLAG_DLIGHTS };		// r1-only
float		ps_r1_lmodel_lerp			= 0.1f	;

// R1-specific
int			ps_r1_GlowsPerFrame			= 16	;					// r1-only

// R2
float		ps_r2_ssaDISCARD			= 4.f	;
float		ps_r2_ssaDONTSORT			= 32.f	;
float		ps_r2_ssaLOD_A				= 48.f	;
float		ps_r2_ssaLOD_B				= 32.f	;
float		ps_r2_ssaHZBvsTEX			= 256.f	;
int			ps_r2_tf_Anisotropic		= 4		;
float		ps_r2_tf_Mipbias			= 0.0f	;

// R2-specific
Flags32		ps_r2_ls_flags				= { R2FLAG_SUN | R2FLAG_FASTBLOOM | R2FLAG_AA };	// r2-only
float		ps_r2_df_parallax_h			= 0.02f;
float		ps_r2_df_parallax_range		= 50.f;
float		ps_r2_tonemap_middlegray	= 0.01f;			// r2-only
float		ps_r2_tonemap_adaptation	= 5.f;				// r2-only
float		ps_r2_tonemap_low_lum		= 0.001f;			// r2-only
float		ps_r2_tonemap_amount		= 0.5f;				// r2-only
float		ps_r2_ls_bloom_kernel_g		= 3.0f;				// r2-only
float		ps_r2_ls_bloom_kernel_b		= .6f;				// r2-only
float		ps_r2_ls_bloom_speed		= 10.f;				// r2-only
float		ps_r2_ls_bloom_kernel_scale	= 1.5f;				// r2-only	// gauss
float		ps_r2_ls_dsm_kernel			= .7f;				// r2-only
float		ps_r2_ls_psm_kernel			= .7f;				// r2-only
float		ps_r2_ls_ssm_kernel			= .7f;				// r2-only
float		ps_r2_ls_bloom_threshold	= .3f;				// r2-only
Fvector		ps_r2_aa_barier				= { .8f, .1f, 0};	// r2-only
Fvector		ps_r2_aa_weight				= { .25f,.25f,0};	// r2-only
float		ps_r2_aa_kernel				= .5f;				// r2-only
float		ps_r2_mblur					= .5f;				// .5f
int			ps_r2_GI_depth				= 1;				// 1..5
int			ps_r2_GI_photons			= 16;				// 8..64
float		ps_r2_GI_clip				= EPS_L;			// EPS
float		ps_r2_GI_refl				= .9f;				// .9f
float		ps_r2_ls_depth_scale		= 1.00001f;			// 1.00001f
float		ps_r2_ls_depth_bias			= -0.0001f;			// -0.0001f
float		ps_r2_ls_squality			= 1.0f;				// 1.00f
float		ps_r2_sun_tsm_projection	= 0.18f;			// 0.18f
float		ps_r2_sun_tsm_bias			= -0.05f;			// 
float		ps_r2_sun_near				= 12.f;				// 12.0f
float		ps_r2_sun_near_border		= 0.66f;			// 1.0f
float		ps_r2_sun_depth_far_scale	= 1.00000f;			// 1.00001f
float		ps_r2_sun_depth_far_bias	= 0.00000f;			// -0.0000f
float		ps_r2_sun_depth_near_scale	= 1.00001f;			// 1.00001f
float		ps_r2_sun_depth_near_bias	= -0.00004f;		// -0.00005f
float		ps_r2_sun_lumscale			= 0.666f;			// 1.0f
float		ps_r2_gmaterial				= 0.f;				// 
float		ps_r2_zfill					= 0.1f;				// .1f

#ifndef _EDITOR
#include	"..\xr_ioconsole.h"
#include	"..\xr_ioc_cmd.h"

//-----------------------------------------------------------------------
class CCC_tf_Aniso	: public CCC_Integer
{
public:
	CCC_tf_Aniso(LPCSTR N, int*	v) : CCC_Integer(N, v, 1, 16)		{ };
	virtual void Execute(LPCSTR args)
	{
		CCC_Integer::Execute	(args);
		for (u32 i=0; i<HW.Caps.raster.dwStages; i++)
			CHK_DX(HW.pDevice->SetSamplerState( i, D3DSAMP_MAXANISOTROPY, *value	));
	}
};
class CCC_tf_MipBias: public CCC_Float
{
public:
	CCC_tf_MipBias(LPCSTR N, float*	v) : CCC_Float(N, v, -3, +3)	{ };
	virtual void Execute(LPCSTR args)
	{
		CCC_Float::Execute	(args);
		for (u32 i=0; i<HW.Caps.raster.dwStages; i++)
			CHK_DX(HW.pDevice->SetSamplerState( i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) value)));
	}
};
class CCC_R2GM		: public CCC_Float
{
public:
	CCC_R2GM(LPCSTR N, float*	v) : CCC_Float(N, v, 0.f, 4.f) { *v = 0; };
	virtual void	Execute	(LPCSTR args)
	{
		if (0==xr_strcmp(args,"on"))	{
			ps_r2_ls_flags.set	(R2FLAG_GLOBALMATERIAL,TRUE);
		} else if (0==xr_strcmp(args,"off"))	{
			ps_r2_ls_flags.set	(R2FLAG_GLOBALMATERIAL,FALSE);
		} else {
			CCC_Float::Execute	(args);
			if (ps_r2_ls_flags.test(R2FLAG_GLOBALMATERIAL))	{
				static LPCSTR	name[4]	=	{ "oren", "blin", "phong", "metal" };
				float	mid		= *value	;
				int		m0		= iFloor(mid)	% 4;
				int		m1		= (m0+1)		% 4;
				float	frc		= mid - float(iFloor(mid));
				Msg		("* material set to [%s]-[%s], with lerp of [%f]",name[m0],name[m1],frc);
			}
		}
	}
};
class CCC_Screenshot : public IConsole_Command
{
public:
	CCC_Screenshot(LPCSTR N) : IConsole_Command(N)  { };
	virtual void Execute(LPCSTR args) {
		string_path	name;	name[0]=0;
		sscanf		(args,"%s",	name);
		LPCSTR		image	= xr_strlen(name)?name:0;
		::Render->Screenshot(IRender_interface::SM_NORMAL,image);
	}
};
class CCC_ModelPoolStat : public IConsole_Command
{
public:
	CCC_ModelPoolStat(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = TRUE; };
	virtual void Execute(LPCSTR args) {
		RImplementation.Models->dump();
	}
};
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
void		xrRender_initconsole	()
{
	// Common
	CMD1(CCC_Screenshot,"screenshot"			);

//	CMD4(CCC_Integer,	"r__supersample",		&ps_r__Supersample,			1,		4		);
	CMD4(CCC_Integer,	"r__lsleep_frames",		&ps_r__LightSleepFrames,	4,		30		);

	CMD1(CCC_ModelPoolStat,"stat_models"		);

	Fvector	tw_min,tw_max;
	CMD4(CCC_Float,		"r__wallmark_ttl",		&ps_r__WallmarkTTL,			1.0f,	5.f*60.f);
	CMD4(CCC_Float,		"r__ssa_glod_start",	&ps_r__GLOD_ssa_start,		128,	512		);
	CMD4(CCC_Float,		"r__ssa_glod_end",		&ps_r__GLOD_ssa_end,		16,		96		);
	CMD4(CCC_Float,		"r__geometry_lod",		&ps_r__LOD,					0,		2		);
	CMD4(CCC_Float,		"r__geometry_lod_pow",	&ps_r__LOD_Power,			0,		2		);
	
#ifdef DEBUG
	CMD4(CCC_Float,		"r__detail_l_ambient",	&ps_r__Detail_l_ambient,	.5f,	.95f	);
	CMD4(CCC_Float,		"r__detail_l_aniso",	&ps_r__Detail_l_aniso,		.1f,	.5f		);
	CMD4(CCC_Float,		"r__detail_density",	&ps_r__Detail_density,		.05f,	0.99f	);

	tw_min.set			(EPS,EPS,EPS);
	tw_max.set			(2,2,2);
	CMD4(CCC_Float,		"r__d_tree_w_rot",		&ps_r__Tree_w_rot,			.01f,	100.f	);
	CMD4(CCC_Float,		"r__d_tree_w_speed",	&ps_r__Tree_w_speed,		1.0f,	10.f	);
	CMD4(CCC_Float,		"r__d_tree_w_amp",		&ps_r__Tree_w_amp,			.001f,	1.f		);
	CMD4(CCC_Vector3,	"r__d_tree_wave",		&ps_r__Tree_Wave,			tw_min, tw_max	);
#endif

	// R1
	CMD4(CCC_Float,		"r1_ssa_lod_a",			&ps_r1_ssaLOD_A,			16,		96		);
	CMD4(CCC_Float,		"r1_ssa_lod_b",			&ps_r1_ssaLOD_B,			16,		64		);
	CMD4(CCC_Float,		"r1_ssa_discard",		&ps_r1_ssaDISCARD,			1,		16		);
	CMD4(CCC_Float,		"r1_ssa_dontsort",		&ps_r1_ssaDONTSORT,			16,		128		);
	CMD4(CCC_Float,		"r1_ssa_hzb_vs_tex",	&ps_r1_ssaHZBvsTEX,			16,		512		);
	CMD4(CCC_Float,		"r1_lmodel_lerp",		&ps_r1_lmodel_lerp,			0,		1		);
	CMD2(CCC_tf_Aniso,	"r1_tf_aniso",			&ps_r1_tf_Anisotropic		);
	CMD2(CCC_tf_MipBias,"r1_tf_mipbias",		&ps_r1_tf_Mipbias			);
	CMD3(CCC_Mask,		"r1_dlights",			&ps_r1_flags,				R1FLAG_DLIGHTS	);

	// R1-specific
	CMD4(CCC_Integer,	"r1_glows_per_frame",	&ps_r1_GlowsPerFrame,		2,		32		);

	// R2
	CMD4(CCC_Float,		"r2_ssa_lod_a",			&ps_r2_ssaLOD_A,			16,		96		);
	CMD4(CCC_Float,		"r2_ssa_lod_b",			&ps_r2_ssaLOD_B,			32,		64		);
	CMD4(CCC_Float,		"r2_ssa_discard",		&ps_r2_ssaDISCARD,			1,		16		);
	CMD4(CCC_Float,		"r2_ssa_dontsort",		&ps_r2_ssaDONTSORT,			16,		128		);
	CMD4(CCC_Float,		"r2_ssa_hzb_vs_tex",	&ps_r2_ssaHZBvsTEX,			16,		512		);
	CMD2(CCC_tf_Aniso,	"r2_tf_aniso",			&ps_r2_tf_Anisotropic		);
	CMD2(CCC_tf_MipBias,"r2_tf_mipbias",		&ps_r2_tf_Mipbias			);

	// R2-specific
	CMD2(CCC_R2GM,		"r2em",					&ps_r2_gmaterial							);
	CMD3(CCC_Mask,		"r2_tonemap",			&ps_r2_ls_flags,			R2FLAG_TONEMAP	);
	CMD4(CCC_Float,		"r2_tonemap_middlegray",&ps_r2_tonemap_middlegray,	0.0f,	2.0f	);
	CMD4(CCC_Float,		"r2_tonemap_adaptation",&ps_r2_tonemap_adaptation,	0.01f,	10.0f	);
	CMD4(CCC_Float,		"r2_tonemap_lowlum",	&ps_r2_tonemap_low_lum,		0.0001f,1.0f	);
	CMD4(CCC_Float,		"r2_tonemap_amount",	&ps_r2_tonemap_amount,		0.0000f,1.0f	);
	CMD4(CCC_Float,		"r2_ls_bloom_kernel_scale",&ps_r2_ls_bloom_kernel_scale,	1.f,	2.f);
	CMD4(CCC_Float,		"r2_ls_bloom_kernel_g",	&ps_r2_ls_bloom_kernel_g,	1.f,	7.f		);
	CMD4(CCC_Float,		"r2_ls_bloom_kernel_b",	&ps_r2_ls_bloom_kernel_b,	0.01f,	1.f		);
	CMD4(CCC_Float,		"r2_ls_bloom_threshold",&ps_r2_ls_bloom_threshold,	0.f,	1.f		);
	CMD4(CCC_Float,		"r2_ls_bloom_speed",	&ps_r2_ls_bloom_speed,		0.f,	100.f	);
	CMD3(CCC_Mask,		"r2_ls_bloom_fast",		&ps_r2_ls_flags,			R2FLAG_FASTBLOOM);
	CMD4(CCC_Float,		"r2_ls_dsm_kernel",		&ps_r2_ls_dsm_kernel,		.1f,	3.f		);
	CMD4(CCC_Float,		"r2_ls_psm_kernel",		&ps_r2_ls_psm_kernel,		.1f,	3.f		);
	CMD4(CCC_Float,		"r2_ls_ssm_kernel",		&ps_r2_ls_ssm_kernel,		.1f,	3.f		);
	CMD4(CCC_Float,		"r2_ls_squality",		&ps_r2_ls_squality,			.001f,	3.f		);

	CMD3(CCC_Mask,		"r2_zfill",				&ps_r2_ls_flags,			R2FLAG_ZFILL	);
	CMD4(CCC_Float,		"r2_zfill_depth",		&ps_r2_zfill,				.001f,	.5f		);

	CMD3(CCC_Mask,		"r2_sun",				&ps_r2_ls_flags,			R2FLAG_SUN		);
	CMD3(CCC_Mask,		"r2_sun_details",		&ps_r2_ls_flags,			R2FLAG_SUN_DETAILS);
	CMD3(CCC_Mask,		"r2_sun_focus",			&ps_r2_ls_flags,			R2FLAG_SUN_FOCUS);
	CMD3(CCC_Mask,		"r2_sun_tsm",			&ps_r2_ls_flags,			R2FLAG_SUN_TSM	);
	CMD4(CCC_Float,		"r2_sun_tsm_proj",		&ps_r2_sun_tsm_projection,	.001f,	0.8f	);
	CMD4(CCC_Float,		"r2_sun_tsm_bias",		&ps_r2_sun_tsm_bias,		-0.5,	+0.5	);
	CMD4(CCC_Float,		"r2_sun_near",			&ps_r2_sun_near,			1.f,	50.f	);
	CMD4(CCC_Float,		"r2_sun_near_border",	&ps_r2_sun_near_border,		.5f,	1.0f	);
	CMD4(CCC_Float,		"r2_sun_depth_far_scale",&ps_r2_sun_depth_far_scale,0.5,	1.5		);
	CMD4(CCC_Float,		"r2_sun_depth_far_bias",&ps_r2_sun_depth_far_bias,	-0.5,	+0.5	);
	CMD4(CCC_Float,		"r2_sun_depth_near_scale",&ps_r2_sun_depth_near_scale,0.5,	1.5		);
	CMD4(CCC_Float,		"r2_sun_depth_near_bias",&ps_r2_sun_depth_near_bias,-0.5,	+0.5	);
	CMD4(CCC_Float,		"r2_sun_lumscale",		&ps_r2_sun_lumscale,		-1.0,	+1.0	);

	CMD3(CCC_Mask,		"r2_aa",				&ps_r2_ls_flags,			R2FLAG_AA);
	CMD4(CCC_Float,		"r2_aa_kernel",			&ps_r2_aa_kernel,			0.3f,	0.7f	);
	CMD4(CCC_Float,		"r2_mblur",				&ps_r2_mblur,				0.0f,	1.0f	);

	CMD3(CCC_Mask,		"r2_gi",				&ps_r2_ls_flags,			R2FLAG_GI);
	CMD4(CCC_Float,		"r2_gi_clip",			&ps_r2_GI_clip,				EPS,	0.1f	);
	CMD4(CCC_Integer,	"r2_gi_depth",			&ps_r2_GI_depth,			1,		5		);
	CMD4(CCC_Integer,	"r2_gi_photons",		&ps_r2_GI_photons,			8,		256		);
	CMD4(CCC_Float,		"r2_gi_refl",			&ps_r2_GI_refl,				EPS_L,	0.99f	);

	CMD4(CCC_Float,		"r2_ls_depth_scale",	&ps_r2_ls_depth_scale,		0.5,	1.5		);
	CMD4(CCC_Float,		"r2_ls_depth_bias",		&ps_r2_ls_depth_bias,		-0.5,	+0.5	);

	CMD4(CCC_Float,		"r2_parallax_h",		&ps_r2_df_parallax_h,		.0f,	.5f		);
	CMD4(CCC_Float,		"r2_parallax_range",	&ps_r2_df_parallax_range,	5.0f,	75.0f	);

	tw_min.set			(0,0,0);	tw_max.set	(1,1,1);
	CMD4(CCC_Vector3,	"r2_aa_break",			&ps_r2_aa_barier,			tw_min, tw_max	);

	tw_min.set			(0,0,0);	tw_max.set	(1,1,1);
	CMD4(CCC_Vector3,	"r2_aa_weight",			&ps_r2_aa_weight,			tw_min, tw_max	);
}
#endif
