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
	case CRender::PHASE_SMAP_D:	// smap-direct
	case CRender::PHASE_SMAP_P:	// smap-point
	case CRender::PHASE_SMAP_S:	// smap-spot									//. !!!! "shadow_direct_base_aref"
		if (RImplementation.o.HW_smap)	C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE,TRUE,TRUE,FALSE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,220);
		else							C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	case 4: 	// deffer-EMAP
		break;
	}
}
