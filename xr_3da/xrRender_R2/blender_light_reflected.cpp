#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_reflected.h"

CBlender_accum_reflected::CBlender_accum_reflected	()	{	description.CLS		= 0;	}
CBlender_accum_reflected::~CBlender_accum_reflected	()	{	}

void	CBlender_accum_reflected::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	C.r_Pass			("accum_volume",	"accum_reflected",false,	FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
	C.r_Sampler_rtf		("s_position",		r2_RT_P);
	C.r_Sampler_rtf		("s_normal",		r2_RT_N);
	C.r_Sampler_clf		("s_material",		r2_material);
	C.r_End				();
	break;
}
