#include "stdafx.h"
#pragma hdrstop

#include "Blender_combine.h"

CBlender_combine::CBlender_combine	()	{	description.CLS		= 0;	}
CBlender_combine::~CBlender_combine	()	{	}
 
void	CBlender_combine::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:
		C.r2_Pass				("null",			r2s("r2_combine_2"),	FALSE,	FALSE);
		C.r2_Sampler			("s_bloom",			r2_RT_bloom1,	D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
		C.r2_Sampler			("s_sky",			r2_RT_bloom2,	D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
		C.r2_End				();
		break;
	case 1:
	case 2:
	case 3:
		C.r2_Pass				("null",			r2s("r2_combine_1"),	FALSE,	FALSE, TRUE, D3DBLEND_INVSRCALPHA, D3DBLEND_SRCALPHA);
		C.r2_Sampler_rtf		("s_position",		r2_RT_P);
		C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
		C.r2_Sampler_rtf		("s_color",			r2_RT_D_G);
		C.r2_Sampler_rtf		("s_accumulator",	r2_RT_accum);
		C.r2_Sampler			("s_bloom",			r2_RT_bloom1,	D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
		C.r2_Sampler			("s_sky",			r2_RT_bloom2,	D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
		C.r2_End				();
		break;
	}
}
