#include "stdafx.h"
#pragma hdrstop

#include "Blender_combine.h"

CBlender_combine::CBlender_combine	()	{	description.CLS		= 0;	}
CBlender_combine::~CBlender_combine	()	{	}
 
void	CBlender_combine::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	C.r2_Pass				("null",			"r2_combine",	FALSE,	FALSE);
	C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
	C.r2_Sampler_rtf		("s_color",			r2_RT_D_G);
	C.r2_Sampler_rtf		("s_accumulator",	r2_RT_accum);
	C.r2_Sampler			("s_bloom",			r2_RT_bloom1,	D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
	C.r2_End				();
}
