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
	case CRender::PHASE_SMAP_D:	// smap-direct
	case CRender::PHASE_SMAP_P:	// smap-point
	case CRender::PHASE_SMAP_S:	// smap-spot							//. !!!! dumb
		if (RImplementation.o.HW_smap)	C.r_Pass	("shadow_direct_base","depth",	FALSE,TRUE,TRUE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE);
		else							C.r_Pass	("shadow_direct_base","shadow_direct_base",FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	case 4: 	// deffer-emap
		break;
	}
}
