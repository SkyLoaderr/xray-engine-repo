#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_mask.h"

CBlender_accum_direct_mask::CBlender_accum_direct_mask	()	{	description.CLS		= 0;	}
CBlender_accum_direct_mask::~CBlender_accum_direct_mask	()	{	}

void	CBlender_accum_direct_mask::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	switch (C.iElement) 
	{
	case SE_SPOT_MASK:		// 
		C.r_Pass			("accum_mask",		"dumb",		false,	TRUE,FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_End				();
		break;
	case 1:	// mask downsample
		//C.r_Pass			("null",			"bloom_mask_filter",FALSE,FALSE,FALSE);
		//C.r_Sampler_rtf	("s_accumulator",	r2_RT_accum);
		//C.r_End			();
		break;
	case 2:	// stencil mask for directional light
		C.r_Pass			("null",			"accum_direct_mask",FALSE,FALSE,FALSE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,1);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N);
		C.r_End				();
		break;
	}
}
