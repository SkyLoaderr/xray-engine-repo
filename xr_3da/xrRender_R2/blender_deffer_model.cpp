#include "stdafx.h"
#pragma hdrstop

#include "Blender_deffer_model.h"

CBlender_deffer_model::CBlender_deffer_model	()	{	description.CLS		= B_MODEL;	}
CBlender_deffer_model::~CBlender_deffer_model	()	{	}

void	CBlender_deffer_model::Save	(	IWriter& fs )
{
	IBlender::Save	(fs);
}
void	CBlender_deffer_model::Load	(	IReader& fs, WORD version )
{
	IBlender::Load	(fs,version);
}

void	CBlender_deffer_model::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	// codepath is the same, only the shaders differ
	switch(C.iElement) 
	{
	case 0: 	// deffer
		C.r_Pass		(r2v("r2_deffer_model_flat"),r2p("r2_deffer_base_flat"),FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	case 1:		// smap-direct
		if (RImplementation.b_nv3x)	C.r_Pass	(r2v("r2_shadow_direct_base"),r2p("r2_shadow_direct_base"),FALSE,TRUE,TRUE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE);
		else						C.r_Pass	(r2v("r2_shadow_direct_base"),r2p("r2_shadow_direct_base"),FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	case 2:		// smap-point
		C.r_Pass		(r2v("r2_shadow_point_base"),r2p("r2_shadow_point_base"),FALSE);
		C.r_Sampler		("s_base",			C.L_textures[0]);
		C.r_Constant	("light_position",	&RImplementation.Binders.l_position);
		C.r_End			();
		break;
	case 3:		// smap-spot
		if (RImplementation.b_nv3x)	C.r_Pass			(r2v("r2_shadow_spot_base"),r2p("r2_shadow_direct_base"),FALSE,TRUE,TRUE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE);
		else						C.r_Pass			(r2v("r2_shadow_spot_base"),r2p("r2_shadow_direct_base"),FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	}
}
