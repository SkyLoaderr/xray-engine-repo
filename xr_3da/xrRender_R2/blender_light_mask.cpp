#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_mask.h"

CBlender_accum_direct_mask::CBlender_accum_direct_mask	()	{	description.CLS		= 0;	}
CBlender_accum_direct_mask::~CBlender_accum_direct_mask	()	{	}

void	CBlender_accum_direct_mask::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:		// near pass
		C.r2_Pass				("null",		"r2_accum_direct_1",TRUE,FALSE,FALSE);
		C.RS.SetRS				(D3DRS_ZFUNC,	D3DCMP_GREATEREQUAL);
		C.r2_Sampler			("s_position",	r2_RT_P,		D3DTADDRESS_CLAMP,D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT);
		C.r2_Sampler			("s_smap",		r2_RT_smap_d,	D3DTADDRESS_CLAMP,D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT);
		C.r2_End				();
		break;
	case 1:		// far pass
		C.r2_Pass				("null",		"r2_accum_direct_2",FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		C.r2_Sampler			("s_position",	r2_RT_P,		D3DTADDRESS_CLAMP,D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT);
		C.r2_Sampler			("s_smap",		r2_RT_smap_d,	D3DTADDRESS_CLAMP,D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT);
		C.r2_End				();
		break;
	case 2:		// lighting pass
		C.r2_Pass				("null",		"r2_accum_direct_L",FALSE,FALSE,TRUE,	D3DBLEND_DESTCOLOR,		D3DBLEND_ZERO);
		C.r2_Sampler			("s_position",	r2_RT_P,		D3DTADDRESS_CLAMP,D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT);
		C.r2_Sampler			("s_normal",	r2_RT_N,		D3DTADDRESS_CLAMP,D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT);
		C.r2_End				();
		break;
	}
}
