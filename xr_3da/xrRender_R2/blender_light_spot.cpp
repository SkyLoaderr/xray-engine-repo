#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_spot.h"

CBlender_accum_spot::CBlender_accum_spot	()	{	description.CLS		= 0;	}
CBlender_accum_spot::~CBlender_accum_spot	()	{	}

void	CBlender_accum_spot::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	if (RImplementation.b_nv3x)
	{
		// two positions, depth as texture, filtered
		switch (C.iElement)
		{
		case 0:	// masking
			C.r_Pass			("accum_spot_mask",	"accum_spot_mask",TRUE,FALSE);
			C.r_End				();
			break;
		case 1:	// lighting/shadowing - front/back
			C.r_Pass			("accum_spot",	"accum_spot_unmask",FALSE,FALSE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,4);
			C.r_Sampler_rtf		("s_normal",		r2_RT_N_H);
			C.r_End				();
			break;
		case 2: // lighting/shadowing - front/back
		case 3:
			C.r_Pass			("accum_spot",	"accum_spot_prj",FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
			C.r_Sampler_rtf		("s_position",		r2_RT_P);
			C.r_Sampler_rtf		("s_normal",		r2_RT_N_H);
			C.r_Sampler_clf		("s_material",		r2_material);
			C.r_Sampler_clf		("s_lightmap",		C.L_textures[0]);
			C.r_Sampler_clf		("s_smap",			r2_RT_smap_d_depth);
			C.r_End				();
			break;
		}
	}
	else
	{
		switch (C.iElement)
		{
		case 0:	// masking
			/*
			C.r_Pass			("accum_spot_mask"),	"accum_spot_mask"),TRUE,FALSE);
			C.r_Sampler_rtf		("s_position",		r2_RT_P);
			C.r_Sampler_rtf		("s_normal",		r2_RT_N_H);
			C.r_Sampler_rtf		("s_smap",			r2_RT_smap_d_surf);
			C.r_End				();
			break;
			*/
		case 1:	// lighting/shadowing - front/back
		case 2:
		case 3:
			C.r_Pass			("null",			"accum_spot",FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
			C.r_Sampler_rtf		("s_position",		r2_RT_P);
			C.r_Sampler_rtf		("s_normal",		r2_RT_N_H);
			C.r_Sampler_rtf		("s_smap",			r2_RT_smap_d_surf);
			C.r_Sampler_clf		("s_material",		r2_material);
			C.r_Sampler_clf		("s_lightmap",		"lights\\lights_strobevent01");
			C.r_End				();
			break;
		}
	}
}
