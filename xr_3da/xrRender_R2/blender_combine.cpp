#include "stdafx.h"
#pragma hdrstop

#include "Blender_combine.h"

CBlender_combine::CBlender_combine	()	{	description.CLS		= 0;	}
CBlender_combine::~CBlender_combine	()	{	}
 
void	CBlender_combine::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:	// combine
		C.r_Pass			("null",			"combine_1",		FALSE,	FALSE,	FALSE); // , TRUE, D3DBLEND_INVSRCALPHA, D3DBLEND_SRCALPHA);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N);
		C.r_Sampler_rtf		("s_diffuse",		r2_RT_albedo);
		C.r_Sampler_rtf		("s_accumulator",	r2_RT_accum);
		C.r_Sampler_rtf		("s_depth",			r2_RT_depth);
		C.r_Sampler_rtf		("s_tonemap",		r2_RT_luminance_cur);
		C.r_End				();
		break;
	case 1:	// aa-edge-detection + AA :)
		C.r_Pass			("null",			"combine_2",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N);
		C.r_Sampler_clf		("s_image",			r2_RT_generic0);
		C.r_Sampler_clf		("s_bloom",			r2_RT_bloom1);
		C.r_Sampler_clf		("s_distort",		r2_RT_generic1);
		C.r_End				();
		break;
	case 2:	// non-AA
		C.r_Pass			("null",			"combine_2noaa",	FALSE,	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N);
		C.r_Sampler_clf		("s_image",			r2_RT_generic0);
		C.r_Sampler_clf		("s_bloom",			r2_RT_bloom1);
		C.r_Sampler_clf		("s_distort",		r2_RT_generic1);
		C.r_End				();
		break;
	case 3:	// aa-edge-detection + AA :) + DISTORTION
		C.r_Pass			("null",			"combine_2D",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N);
		C.r_Sampler_clf		("s_image",			r2_RT_generic0);
		C.r_Sampler_clf		("s_bloom",			r2_RT_bloom1);
		C.r_Sampler_clf		("s_distort",		r2_RT_generic1);
		C.r_End				();
		break;
	case 4:	// non-AA + DISTORTION
		C.r_Pass			("null",			"combine_2Dnoaa",	FALSE,	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N);
		C.r_Sampler_clf		("s_image",			r2_RT_generic0);
		C.r_Sampler_clf		("s_bloom",			r2_RT_bloom1);
		C.r_Sampler_clf		("s_distort",		r2_RT_generic1);
		C.r_End				();
		break;
	}
}
