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
	case 0:	// sky
		C.r_Pass			("null",			r2p("combine_2"),	FALSE,	FALSE);
		C.r_Sampler_clf		("s_bloom",			r2_RT_bloom1);
		C.r_Sampler_clf		("s_sky",			r2_RT_bloom2);
		C.r_End				();
		break;
	case 1:	// level
		C.r_Pass			("null",			r2p("combine_1"),	FALSE,	FALSE, TRUE, D3DBLEND_INVSRCALPHA, D3DBLEND_SRCALPHA);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N_H);
		C.r_Sampler_rtf		("s_color",			r2_RT_D_G);
		C.r_Sampler_rtf		("s_accumulator",	r2_RT_accum);
		C.r_Sampler_clf		("s_bloom",			r2_RT_bloom1);
		C.r_Sampler_clf		("s_sky",			r2_RT_bloom2);
		C.r_End				();
		break;
	case 2:	// aa-blur
		C.r_Pass			("null",			r2p("combine_blur"),	FALSE,	FALSE);
		C.r_Sampler_clf		("s_image",			r2_RT_generic);
		C.r_End				();
		break;
	case 3:	// aa-edge-detection + AA :)
		C.r_Pass			("null",			r2p("combine_aa"),	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N_H);
		C.r_Sampler_clf		("s_image",			r2_RT_generic);
		C.r_Sampler_clf		("s_image_aa",		r2_RT_accum);
		C.r_End				();
		break;
	}
}
