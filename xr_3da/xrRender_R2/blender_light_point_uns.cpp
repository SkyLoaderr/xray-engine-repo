#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_point_uns.h"

CBlender_accum_point_uns::CBlender_accum_point_uns	()	{	description.CLS		= 0;	}
CBlender_accum_point_uns::~CBlender_accum_point_uns	()	{	}

void	CBlender_accum_point_uns::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:	// masking
		C.r2_Pass				(r2s("r2_accum_point_mask"),	r2s("r2_accum_point_mask"),TRUE,FALSE);
		C.r2_Sampler_rtf		("s_position",		r2_RT_P);
		C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
		C.r2_End				();
		break;
	case 1:	// lighting/shadowing - front/back
	case 2:
	case 3:
		C.r2_Pass				(r2s("r2_accum_point"),	r2s("r2_accum_point_uns"),FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		C.r2_Sampler_rtf		("s_position",		r2_RT_P);
		C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
		C.r2_Sampler			("s_material",		r2_material, D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
		C.r2_End				();
		break;
	}
}
