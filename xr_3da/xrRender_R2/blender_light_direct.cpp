#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_direct.h"

CBlender_accum_direct::CBlender_accum_direct	()	{	description.CLS		= 0;	}
CBlender_accum_direct::~CBlender_accum_direct	()	{	}

void	CBlender_accum_direct::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	BOOL	b_HW_smap		= RImplementation.o.HW_smap;
	switch (C.iElement)
	{
	case SE_SUN_NEAR:		// near pass - enable Z-test to perform depth-clipping
		C.r_Pass			("null",			"accum_sun",	false,	TRUE,	FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		C.PassSET_ZB		(TRUE,FALSE,TRUE	);	// force inverted Z-Buffer
		C.r_Sampler_rtf		("s_position",		r2_RT_P			);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N			);
		C.r_Sampler_clf		("s_material",		r2_material		);
		if (b_HW_smap)		C.r_Sampler_clf		("s_smap",r2_RT_smap_depth	);
		else				C.r_Sampler_rtf		("s_smap",r2_RT_smap_surf	);
		jitter				(C);
		C.r_End				();
		break;
	case SE_SUN_FAR:		// far pass, only stencil clipping performed
		C.r_Pass			("null",			"accum_sun",	false,	FALSE,	FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P			);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N			);
		C.r_Sampler_clf		("s_material",		r2_material		);
		if (b_HW_smap)		C.r_Sampler_clf		("s_smap",r2_RT_smap_depth	);
		else				C.r_Sampler_rtf		("s_smap",r2_RT_smap_surf	);
		jitter				(C);
		C.r_End				();
		break;
	}
}
