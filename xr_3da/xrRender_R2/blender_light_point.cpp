#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_point.h"

CBlender_accum_point::CBlender_accum_point	()	{	description.CLS		= 0;	}
CBlender_accum_point::~CBlender_accum_point	()	{	}

void	CBlender_accum_point::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:	// masking
		C.r2_Pass				("r2_accum_point_mask",	"r2_accum_point_mask",TRUE,FALSE);
		C.r2_Sampler_rtf		("s_position",		r2_RT_P);
		C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
		C.r2_Sampler_rtf		("s_smap",			r2_RT_smap_p);
		C.r2_End				();
		break;
	case 1:	// lighting/shadowing - front
		C.r2_Pass				("r2_accum_point",	"r2_accum_point",FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		C.r2_Sampler_rtf		("s_position",		r2_RT_P);
		C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
		C.r2_Sampler_rtf		("s_smap",			r2_RT_smap_p);
		C.r2_End				();
		break;
	case 2:	// lighting/shadowing - back
		C.r2_Pass				("r2_accum_point",	"r2_accum_point",FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		//C.RS.SetRS				(D3DRS_ZFUNC,		D3DCMP_GREATEREQUAL);		// reverse Z-test
		C.r2_Sampler_rtf		("s_position",		r2_RT_P);
		C.r2_Sampler_rtf		("s_normal",		r2_RT_N_H);
		C.r2_Sampler_rtf		("s_smap",			r2_RT_smap_p);
		C.r2_End				();
		break;
	}
}
