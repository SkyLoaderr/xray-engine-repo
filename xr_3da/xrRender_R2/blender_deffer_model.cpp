#include "stdafx.h"
#pragma hdrstop

#include "uber_deffer.h"
#include "Blender_deffer_model.h"

CBlender_deffer_model::CBlender_deffer_model	()	{	description.CLS		= B_MODEL;	}
CBlender_deffer_model::~CBlender_deffer_model	()	{	}

void	CBlender_deffer_model::Save	(	IWriter& fs )
{
	IBlender::Save	(fs);
}
void	CBlender_deffer_model::Load	(	IReader& fs, u16 version )
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
		uber_deffer		(C,"model","base",false);
		break;
	case PHASE_SMAP_D:	// smap-direct
	case PHASE_SMAP_P:	// smap-point
	case PHASE_SMAP_S:	// smap-spot
		if (RImplementation.o.HW_smap)	C.r_Pass	("shadow_direct_model","dumb",					FALSE,TRUE,TRUE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE);
		else							C.r_Pass	("shadow_direct_model","shadow_direct_base",	FALSE);
		C.r_Sampler		("s_base",		C.L_textures[0]);
		C.r_End			();
		break;
	}
}
