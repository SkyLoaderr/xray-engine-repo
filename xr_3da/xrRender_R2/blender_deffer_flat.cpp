#include "stdafx.h"
#pragma hdrstop

#include "uber_deffer.h"
#include "Blender_deffer_flat.h"

CBlender_deffer_flat::CBlender_deffer_flat	()	{	description.CLS		= B_DEFAULT;	}
CBlender_deffer_flat::~CBlender_deffer_flat	()	{	}

void	CBlender_deffer_flat::Save	(	IWriter& fs )
{
	IBlender::Save	(fs);
}
void	CBlender_deffer_flat::Load	(	IReader& fs, u16 version )
{
	IBlender::Load	(fs,version);
}

void	CBlender_deffer_flat::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	// codepath is the same, only the shaders differ
	switch(C.iElement) 
	{
	case 0: 	// deffer
		uber_deffer		(C,"base","base",false);
		break;
	case 1:		// smap-direct
		if (RImplementation.o.HW_smap)	C.r_Pass	("shadow_direct_base","dumb",	FALSE,TRUE,TRUE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE);
		else							C.r_Pass	("shadow_direct_base","shadow_direct_base",FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	case 2:		// smap-point
		C.r_Pass		("shadow_point_base","shadow_point_base",FALSE);
		C.r_Sampler		("s_base",			C.L_textures[0]);
		C.r_End			();
		break;
	case 3:		// smap-spot
		if (RImplementation.o.HW_smap)	C.r_Pass	("shadow_direct_base","dumb",FALSE,TRUE,TRUE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE);
		else							C.r_Pass	("shadow_direct_base","shadow_direct_base",FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	case 4: 	// deffer-emap
		break;
	}
}
