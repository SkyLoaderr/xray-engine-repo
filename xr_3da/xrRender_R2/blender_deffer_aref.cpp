#include "stdafx.h"
#pragma hdrstop

#include "uber_deffer.h"
#include "Blender_deffer_aref.h"

CBlender_deffer_aref::CBlender_deffer_aref	()	{	description.CLS		= B_DEFAULT_AREF;	}
CBlender_deffer_aref::~CBlender_deffer_aref	()	{	}

void	CBlender_deffer_aref::Save	(	IWriter& fs )
{
	IBlender::Save	(fs);
}
void	CBlender_deffer_aref::Load	(	IReader& fs, u16 version )
{
	IBlender::Load	(fs,version);
}

void	CBlender_deffer_aref::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	// codepath is the same, only the shaders differ
	// ***only pixel shaders differ***
	switch(C.iElement) 
	{
	case 0: 	// deffer
		uber_deffer		(C,"base","base",true);
		break;
	case 1:		// smap-direct
		if (RImplementation.b_HW_smap)	C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE,TRUE,TRUE,FALSE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,220);
		else							C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	case 2:		// smap-point
		C.r_Pass		("shadow_point_base","shadow_point_base_aref",FALSE);
		C.r_Sampler		("s_base",			C.L_textures[0]);
		C.r_End			();
		break;
	case 3:		// smap-spot
		if (RImplementation.b_HW_smap)	C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE,TRUE,TRUE,FALSE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,220);
		else							C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	case 4: 	// deffer-EMAP
		break;
	}
}
