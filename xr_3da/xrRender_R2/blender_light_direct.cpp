#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_direct.h"

CBlender_accum_direct::CBlender_accum_direct	()	{	description.CLS		= 0;	}
CBlender_accum_direct::~CBlender_accum_direct	()	{	}

void	CBlender_accum_direct::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	if (RImplementation.b_nv3x)
	{
		switch (C.iElement)
		{
		case 0:		// near pass
			C.r2_Pass				("null",		r2s("r2_accum_direct_1"),TRUE,FALSE,FALSE);
			C.RS.SetRS				(D3DRS_ZFUNC,	D3DCMP_GREATEREQUAL);
			C.r2_Sampler_rtf		("s_position0",	r2_RT_DEFFER, r2_RT_DEFFER_P0	);
			C.r2_Sampler_rtf		("s_position1",	r2_RT_DEFFER, r2_RT_DEFFER_P1	);
			C.r2_Sampler_rtf		("s_smap",		r2_RT_smap_d);
			C.r2_End				();
			break;
		case 1:		// far pass
			C.r2_Pass				("null",		r2s("r2_accum_direct_2"),TRUE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
			C.r2_Sampler_rtf		("s_position0",	r2_RT_DEFFER, r2_RT_DEFFER_P0	);
			C.r2_Sampler_rtf		("s_position1",	r2_RT_DEFFER, r2_RT_DEFFER_P1	);
			C.r2_Sampler_rtf		("s_smap",		r2_RT_smap_d);
			C.r2_Sampler			("s_fade",		r2_ds2_fade, D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
			C.r2_End				();
			break;
		case 2:		// lighting pass
			C.r2_Pass				("null",		r2s("r2_accum_direct_L"),FALSE,FALSE,TRUE,	D3DBLEND_DESTCOLOR,		D3DBLEND_ZERO);
			C.r2_Sampler_rtf		("s_position0",	r2_RT_DEFFER, r2_RT_DEFFER_P0	);
			C.r2_Sampler_rtf		("s_position1",	r2_RT_DEFFER, r2_RT_DEFFER_P1	);
			C.r2_Sampler_rtf		("s_normal",	r2_RT_DEFFER, r2_RT_DEFFER_N_H	);
			C.r2_Sampler			("s_material",	r2_material, D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
			C.r2_End				();
			break;
		}
	} 
	else
	{
		switch (C.iElement)
		{
		case 0:		// near pass
			C.r2_Pass				("null",		r2s("r2_accum_direct_1"),TRUE,FALSE,FALSE);
			C.RS.SetRS				(D3DRS_ZFUNC,	D3DCMP_GREATEREQUAL);
			C.r2_Sampler_rtf		("s_position",	r2_RT_P);
			C.r2_Sampler_rtf		("s_smap",		r2_RT_smap_d);
			C.r2_End				();
			break;
		case 1:		// far pass
			C.r2_Pass				("null",		r2s("r2_accum_direct_2"),TRUE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
			C.r2_Sampler_rtf		("s_position",	r2_RT_P);
			C.r2_Sampler_rtf		("s_smap",		r2_RT_smap_d);
			C.r2_Sampler			("s_fade",		r2_ds2_fade, D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
			C.r2_End				();
			break;
		case 2:		// lighting pass
			C.r2_Pass				("null",		r2s("r2_accum_direct_L"),FALSE,FALSE,TRUE,	D3DBLEND_DESTCOLOR,		D3DBLEND_ZERO);
			C.r2_Sampler_rtf		("s_position",	r2_RT_P);
			C.r2_Sampler_rtf		("s_normal",	r2_RT_N_H);
			C.r2_Sampler			("s_material",	r2_material, D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
			C.r2_End				();
			break;
		}
	}
}
