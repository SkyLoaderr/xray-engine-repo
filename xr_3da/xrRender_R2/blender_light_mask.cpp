#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_mask.h"

CBlender_accum_direct_mask::CBlender_accum_direct_mask	()	{	description.CLS		= 0;	}
CBlender_accum_direct_mask::~CBlender_accum_direct_mask	()	{	}

void	CBlender_accum_direct_mask::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	switch (C.iElement) 
	{
	case 0:	// white mask for skybox
		C.r2_Pass				("null",			"r2_bloom_mask",FALSE,FALSE);
		C.r2_End				();
		break;
	case 1:	// mask downsample
		C.r2_Pass				("null",			"r2_bloom_mask_filter",FALSE,FALSE);
		C.r2_Sampler_rtf		("s_accumulator",	r2_RT_accum);
		C.r2_End				();
		break;
	case 2:	// stencil mask for directional light
		C.r2_Pass				("null",			"r2_accum_direct_mask",FALSE,FALSE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,4);
		C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
		C.r2_End				();
		break;
	}
}
