#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_point.h"

CBlender_accum_point::CBlender_accum_point	()	{	description.CLS		= 0;	}
CBlender_accum_point::~CBlender_accum_point	()	{	}

void	CBlender_accum_point::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	C.r2_Pass				("null",		"r2_accum_direct_1",TRUE,FALSE,FALSE);
	C.r2_Sampler_rtf		("s_position",	r2_RT_P);
	C.r2_Sampler_rtf		("s_normal",	r2_RT_N);
	C.r2_Sampler_rtf		("s_smap",		r2_RT_smap_d);
	C.r2_End				();
}
