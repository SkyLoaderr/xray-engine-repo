#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_point.h"

CBlender_accum_point::CBlender_accum_point	()	{	description.CLS		= 0;	}
CBlender_accum_point::~CBlender_accum_point	()	{	}

void	CBlender_accum_point::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	if (RImplementation.b_nv3x)
	{
		switch (C.iElement)
		{
		case 0:	// masking
			C.r2_Pass				(r2s("r2_accum_point_mask"),	r2s("r2_accum_point_mask"),TRUE,FALSE);
			C.r2_Sampler_rtf		("s_position0",		r2_RT_DEFFER, r2_RT_DEFFER_P0);
			C.r2_Sampler_rtf		("s_position1",		r2_RT_DEFFER, r2_RT_DEFFER_P1);
			C.r2_Sampler_rtf		("s_normal",		r2_RT_DEFFER, r2_RT_DEFFER_N_H);
			C.r2_Sampler_rtf		("s_smap",			r2_RT_smap_p);
			C.r2_End				();
			break;
		case 1:	// lighting/shadowing - front/back
		case 2:
			C.r2_Pass				(r2s("r2_accum_point"),			r2s("r2_accum_point"),FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
			C.r2_Sampler_rtf		("s_position0",		r2_RT_DEFFER, r2_RT_DEFFER_P0);
			C.r2_Sampler_rtf		("s_position1",		r2_RT_DEFFER, r2_RT_DEFFER_P1);
			C.r2_Sampler_rtf		("s_normal",		r2_RT_DEFFER, r2_RT_DEFFER_N_H);
			C.r2_Sampler_rtf		("s_smap",			r2_RT_smap_p);
			C.r2_Sampler			("s_material",		r2_material, D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
			C.r2_End				();
			break;
		}
	}
	else
	{
		switch (C.iElement)
		{
		case 0:	// masking
			C.r2_Pass				(r2s("r2_accum_point_mask"),	r2s("r2_accum_point_mask"),TRUE,FALSE);
			C.r2_Sampler_rtf		("s_position",		r2_RT_P);
			C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
			C.r2_Sampler_rtf		("s_smap",			r2_RT_smap_p);
			C.r2_End				();
			break;
		case 1:	// lighting/shadowing - front/back
		case 2:
			C.r2_Pass				(r2s("r2_accum_point"),			r2s("r2_accum_point"),FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
			C.r2_Sampler_rtf		("s_position",		r2_RT_P);
			C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
			C.r2_Sampler_rtf		("s_smap",			r2_RT_smap_p);
			C.r2_Sampler			("s_material",		r2_material, D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
			C.r2_End				();
			break;
		}
	}
}
