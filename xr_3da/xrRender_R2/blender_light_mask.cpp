#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_mask.h"

CBlender_accum_direct_mask::CBlender_accum_direct_mask	()	{	description.CLS		= 0;	}
CBlender_accum_direct_mask::~CBlender_accum_direct_mask	()	{	}

void	CBlender_accum_direct_mask::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	C.r2_Pass				("null",		"r2_accum_direct_mask",FALSE,FALSE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,4);
	C.r2_Sampler			("s_normal",	r2_RT_N,		D3DTADDRESS_CLAMP,D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT);
	C.r2_End				();
}
