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
		C.r_Pass			("accum_mask",			"dumb",				false,TRUE,FALSE);
		C.r_Sampler_rtf		("s_position",			r2_RT_P);
		C.r_Sampler_rtf		("s_normal",			r2_RT_N);
		C.r_Sampler_rtf		("s_smap",				r2_RT_smap_p);
		C.r_End				();
		break;
	case 1:	// shadowed
		C.r_Pass			("accum_volume",		"accum_point",		false,FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		C.r_Sampler_rtf		("s_position",			r2_RT_P);
		C.r_Sampler_rtf		("s_normal",			r2_RT_N);
		C.r_Sampler_rtf		("s_smap",				r2_RT_smap_p);
		C.r_Sampler_clf		("s_material",			r2_material);
		C.r_End				();
		break;
	case 2:	// non-shadowed
		C.r_Pass			("accum_volume",		"accum_point_uns",	false,FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		C.r_Sampler_rtf		("s_position",			r2_RT_P);
		C.r_Sampler_rtf		("s_normal",			r2_RT_N);
		C.r_Sampler_rtf		("s_smap",				r2_RT_smap_p);
		C.r_Sampler_clf		("s_material",			r2_material);
		C.r_End				();
		break;
	}
}
