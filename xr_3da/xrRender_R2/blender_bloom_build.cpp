#include "stdafx.h"
#pragma hdrstop

#include "Blender_bloom_build.h"

CBlender_bloom_build::CBlender_bloom_build	()	{	description.CLS		= 0;	}
CBlender_bloom_build::~CBlender_bloom_build	()	{	}
 
void	CBlender_bloom_build::Compile			(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	C.r2_Pass				("null",			"r2_combine",	FALSE,	FALSE);
	C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
	C.r2_Sampler_rtf		("s_color",			r2_RT_D_G);
	C.r2_Sampler_rtf		("s_accumulator",	r2_RT_accum);
	C.r2_End				();
}
