#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_point.h"

CBlender_accum_point::CBlender_accum_point	()	{	description.CLS		= 0;	}
CBlender_accum_point::~CBlender_accum_point	()	{	}

void	CBlender_accum_point::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:	// masking
		C.r_Pass			(r2v("accum_point_mask"),	r2p("accum_point_mask"),TRUE,FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N_H);
		C.r_Sampler_rtf		("s_smap",			r2_RT_smap_p);
		C.r_End				();
		break;
	case 1:	// lighting/shadowing - front/back
	case 2:
		C.r_Pass			(r2v("accum_point"),			r2p("accum_point"),FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N_H);
		C.r_Sampler_rtf		("s_smap",			r2_RT_smap_p);
		C.r_Sampler_clf		("s_material",		r2_material);
		C.r_End				();
		break;
	}
}
