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
		C.r_Pass				("null",			r2p("r2_bloom_build"),	FALSE,	FALSE, TRUE, D3DBLEND_INVSRCALPHA, D3DBLEND_SRCALPHA);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N_H);
		C.r_Sampler_rtf		("s_color",			r2_RT_D_G);
		C.r_Sampler_rtf		("s_accumulator",	r2_RT_accum);
		C.r_Sampler			("s_mask",			r2_RT_bloom2,	D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
		C.r_End				();
		break;
	case 1:		// X-filter
		C.r_Pass				("null",			r2p("r2_bloom_filter"),	FALSE,	FALSE);
		C.r_Sampler			("s_bloom",			r2_RT_bloom1,	D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
		C.r_End				();
		break;
	case 2:		// Y-filter
		C.r_Pass				("null",			r2p("r2_bloom_filter"),	FALSE,	FALSE);
		C.r_Sampler			("s_bloom",			r2_RT_bloom2,	D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
		C.r_End				();
		break;
	}
}
