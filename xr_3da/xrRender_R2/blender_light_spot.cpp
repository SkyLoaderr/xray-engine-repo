#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_spot.h"

CBlender_accum_spot::CBlender_accum_spot	()	{	description.CLS		= 0;	}
CBlender_accum_spot::~CBlender_accum_spot	()	{	}

void	CBlender_accum_spot::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:	// masking
		/*
		C.r2_Pass				("r2_accum_spot_mask",	"r2_accum_spot_mask",TRUE,FALSE);
		C.r2_Sampler_rtf		("s_position",		r2_RT_P);
		C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
		C.r2_Sampler_rtf		("s_smap",			r2_RT_smap_s);
		C.r2_End				();
		break;
		*/
	case 1:	// lighting/shadowing - front/back
	case 2:
		C.r2_Pass				("null",			"r2_accum_spot",FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		C.r2_Sampler_rtf		("s_position",		r2_RT_P);
		C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
		C.r2_Sampler_rtf		("s_smap",			r2_RT_smap_s);
		C.r2_Sampler			("s_material",		r2_material, D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
		C.r2_Sampler			("s_lightmap",		"lights\\lights_strobevent01", D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
		C.r2_End				();
		break;
	}
}
