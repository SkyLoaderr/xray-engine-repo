#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_spot.h"

CBlender_accum_spot::CBlender_accum_spot	()	{	description.CLS		= 0;	}
CBlender_accum_spot::~CBlender_accum_spot	()	{	}

void	CBlender_accum_spot::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	if (RImplementation.b_nv3x)
	{
		// two positions, depth as texture, filtered
		switch (C.iElement)
		{
		case 0:	// masking
			C.r2_Pass				(r2v("r2_accum_spot_mask"),	r2p("r2_accum_spot_mask"),TRUE,FALSE);
			C.r2_End				();
			break;
		case 1:	// lighting/shadowing - front/back
			C.r2_Pass				(r2v("r2_accum_spot"),	r2p("r2_accum_spot_unmask"),FALSE,FALSE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,4);
			C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
			C.r2_End				();
			break;
		case 2: // lighting/shadowing - front/back
		case 3:
			C.r2_Pass				(r2v("r2_accum_spot"),	r2p("r2_accum_spot_prj"),FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
			C.r2_Sampler_rtf		("s_position",		r2_RT_P);
			C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
			C.r2_Sampler			("s_material",		r2_material,					D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
			C.r2_Sampler			("s_lightmap",		C.L_textures[0],				D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
			C.r2_Sampler			("s_smap",			r2_RT_smap_d_depth,				D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
			C.r2_End				();
			break;
		}
	}
	else
	{
		switch (C.iElement)
		{
		case 0:	// masking
			/*
			C.r2_Pass				(r2v("r2_accum_spot_mask"),	r2p("r2_accum_spot_mask"),TRUE,FALSE);
			C.r2_Sampler_rtf		("s_position",		r2_RT_P);
			C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
			C.r2_Sampler_rtf		("s_smap",			r2_RT_smap_d_surf);
			C.r2_End				();
			break;
			*/
		case 1:	// lighting/shadowing - front/back
		case 2:
		case 3:
			C.r2_Pass				("null",			r2p("r2_accum_spot"),FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
			C.r2_Sampler_rtf		("s_position",		r2_RT_P);
			C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
			C.r2_Sampler_rtf		("s_smap",			r2_RT_smap_d_surf);
			C.r2_Sampler			("s_material",		r2_material, D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
			C.r2_Sampler			("s_lightmap",		"lights\\lights_strobevent01", D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
			C.r2_End				();
			break;
		}
	}
}
