#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_spot.h"

CBlender_accum_spot::CBlender_accum_spot	()	{	description.CLS		= 0;	}
CBlender_accum_spot::~CBlender_accum_spot	()	{	}

void	CBlender_accum_spot::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	if (RImplementation.b_HW_smap)
	{
		switch (C.iElement)
		{
		case 0:	// masking
			C.r_Pass			("accum_mask",		"null",		false,	TRUE,FALSE);
			C.r_Sampler_rtf		("s_position",		r2_RT_P);
			C.r_Sampler_rtf		("s_normal",		r2_RT_N_H);
			C.r_Sampler_clf		("s_smap",			r2_RT_smap_d_depth, true);
			C.r_End				();
			break;
		case 1:	// lighting/shadowing - front/back
		case 2:
		case 3:
			C.r_Pass			("accum_volume",	"accum_spot",false,	FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
			C.r_Sampler_rtf		("s_position",		r2_RT_P);
			C.r_Sampler_rtf		("s_normal",		r2_RT_N_H);
			C.r_Sampler_clf		("s_smap",			r2_RT_smap_d_depth, true);
			C.r_Sampler_clf		("s_material",		r2_material);
			C.r_Sampler_clf		("s_attenuate",		r2_attenuate);
			C.r_Sampler_clf		("s_lmap",			C.L_textures[0]);
			C.r_End				();
			break;
		}
	}
	else
	{
		switch (C.iElement)
		{
		case 0:	// masking
			C.r_Pass			("accum_mask",		"null",		false,	TRUE,FALSE);
			C.r_Sampler_rtf		("s_position",		r2_RT_P);
			C.r_Sampler_rtf		("s_normal",		r2_RT_N_H);
			C.r_Sampler_rtf		("s_smap",			r2_RT_smap_d_surf);
			C.r_End				();
			break;
		case 1:	// lighting/shadowing - front/back
		case 2:
		case 3:
			C.r_Pass			("accum_volume",	"accum_spot",false,	FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
			C.r_Sampler_rtf		("s_position",		r2_RT_P);
			C.r_Sampler_rtf		("s_normal",		r2_RT_N_H);
			C.r_Sampler_rtf		("s_smap",			r2_RT_smap_d_surf);
			C.r_Sampler_clf		("s_material",		r2_material);
			C.r_Sampler_clf		("s_attenuate",		r2_attenuate);
			C.r_Sampler_clf		("s_lmap",			C.L_textures[0]);
			C.r_End				();
			break;
		}
	}
}
