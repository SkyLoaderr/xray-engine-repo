#include "stdafx.h"
#pragma hdrstop

#include "Blender_combine.h"

CBlender_combine::CBlender_combine	()	{	description.CLS		= 0;	}
CBlender_combine::~CBlender_combine	()	{	}

void	CBlender_combine::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	C.r2_Pass				("null",			"r2_combine",	FALSE,	FALSE);
	C.r2_Sampler_rtf		("s_color",			r2_RT_N);
	C.r2_Sampler_rtf		("s_color",			r2_RT_D_G);
	C.r2_Sampler_rtf		("s_accumulator",	r2_RT_accum);
	C.r2_End				();
}
