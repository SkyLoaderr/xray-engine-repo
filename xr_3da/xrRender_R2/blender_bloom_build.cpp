#include "stdafx.h"
#pragma hdrstop

#include "Blender_bloom_build.h"

CBlender_bloom_build::CBlender_bloom_build	()	{	description.CLS		= 0;	}
CBlender_bloom_build::~CBlender_bloom_build	()	{	}
 
void	CBlender_bloom_build::Compile			(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	// r3xx
	switch (C.iElement)
	{
	case 0:		// transfer into bloom-target
		C.r_Pass			("null",			"bloom_build",	FALSE,	FALSE,	FALSE, TRUE, D3DBLEND_INVSRCALPHA, D3DBLEND_SRCALPHA);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N_H);
		C.r_Sampler_rtf		("s_diffuse",		r2_RT_D_G);
		C.r_Sampler_rtf		("s_accumulator",	r2_RT_accum);
		C.r_Sampler_clf		("s_mask",			r2_RT_bloom2);
		C.r_End				();
		break;
	case 1:		// X-filter
		C.r_Pass			("null",			"bloom_filter",	FALSE,	FALSE,	FALSE);
		C.r_Sampler_clf		("s_bloom",			r2_RT_bloom1);
		C.r_End				();
		break;
	case 2:		// Y-filter
		C.r_Pass			("null",			"bloom_filter",	FALSE,	FALSE,	FALSE);
		C.r_Sampler_clf		("s_bloom",			r2_RT_bloom2);
		C.r_End				();
		break;
	}
}
