#include "stdafx.h"
#pragma hdrstop

#include "Blender_light_spot.h"

CBlender_accum_spot::CBlender_accum_spot	()	{	description.CLS		= 0;	}
CBlender_accum_spot::~CBlender_accum_spot	()	{	}

void	CBlender_accum_spot::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	BOOL	b_HW_smap				= RImplementation.b_HW_smap;
	switch (C.iElement)
	{
	case SE_SPOT_FILL:			// masking
		C.r_Pass			("null", 			"dumb_tcopy",				false,	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_base",			C.L_textures[0]);
		C.r_End				();
		break;
	case SE_SPOT_UNSHADOWED:	// unshadowed
		C.r_Pass			("accum_volume",	"accum_spot_unshadowed",	false,	FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N);
		C.r_Sampler_clf		("s_material",		r2_material);
		C.r_Sampler_clf		("s_lmap",			C.L_textures[0]);
		if (b_HW_smap)		C.r_Sampler_clf		("s_smap",r2_RT_smap_d_depth);
		else				C.r_Sampler_rtf		("s_smap",r2_RT_smap_d_surf	);
		C.r_End				();
		break;
	case SE_SPOT_NORMAL:		// normal
		C.r_Pass			("accum_volume",	"accum_spot_normal",		false,	FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N);
		C.r_Sampler_clf		("s_material",		r2_material);
		C.r_Sampler_clf		("s_lmap",			C.L_textures[0]);
		if (b_HW_smap)		C.r_Sampler_clf		("s_smap",r2_RT_smap_d_depth);
		else				C.r_Sampler_rtf		("s_smap",r2_RT_smap_d_surf	);
		C.r_End				();
		break;
	case SE_SPOT_FULLSIZE:		// normal-fullsize
		C.r_Pass			("accum_volume",	"accum_spot_fullsize",		false,	FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N);
		C.r_Sampler_clf		("s_material",		r2_material);
		C.r_Sampler_clf		("s_lmap",			C.L_textures[0]);
		if (b_HW_smap)		C.r_Sampler_clf		("s_smap",r2_RT_smap_d_depth);
		else				C.r_Sampler_rtf		("s_smap",r2_RT_smap_d_surf	);
		C.r_End				();
		break;
	case SE_SPOT_TRANSLUENT:	// shadowed + transluency
		C.r_Pass			("accum_volume",	"accum_spot_fullsize",		false,	FALSE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N);
		C.r_Sampler_clf		("s_material",		r2_material);
		C.r_Sampler_clf		("s_lmap",			r2_RT_smap_d_surf);			// diff here
		if (b_HW_smap)		C.r_Sampler_clf		("s_smap",r2_RT_smap_d_depth);
		else				C.r_Sampler_rtf		("s_smap",r2_RT_smap_d_surf	);
		C.r_End				();
		break;
	}
}
