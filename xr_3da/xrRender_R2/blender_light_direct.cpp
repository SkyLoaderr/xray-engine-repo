#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_direct.h"

CBlender_accum_direct::CBlender_accum_direct	()	{	description.CLS		= 0;	}
CBlender_accum_direct::~CBlender_accum_direct	()	{	}

void	CBlender_accum_direct::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:
		C.r2_Pass				("null",		"r2_accum_direct",FALSE,FALSE);
		C.r2_Sampler			("s_position",	r2_RT_P);
		C.r2_Sampler			("s_normal",	r2_RT_N);
		C.r2_Sampler			("s_smap",		r2_RT_smap_d);
		C.r2_End				();
		break;
	case 1:
		C.r2_Pass				("null",		"r2_accum_direct",FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		C.r2_Sampler			("s_position",	r2_RT_P);
		C.r2_Sampler			("s_normal",	r2_RT_N);
		C.r2_Sampler			("s_smap",		r2_RT_smap_d);
		C.r2_End				();
		break;
	}
}
