#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_direct.h"

CBlender_accum_direct::CBlender_accum_direct	()	{	description.CLS		= 0;	}
CBlender_accum_direct::~CBlender_accum_direct	()	{	}

void	CBlender_accum_direct::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:		// near pass
	case 1:		// far pass
	case 2:		// lighting pass
		C.r_Pass			("null",		"accum_sun",	false,	FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		C.r_Sampler_rtf		("s_position",	r2_RT_P			);
		C.r_Sampler_rtf		("s_normal",	r2_RT_N			);
		C.r_Sampler_clf		("s_smap",		r2_RT_smap_depth);
		C.r_Sampler_clf		("s_material",	r2_material		);
		jitter				(C);
		C.r_End				();
		break;
	}
}
