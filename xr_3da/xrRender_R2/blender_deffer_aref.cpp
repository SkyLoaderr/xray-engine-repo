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
	C.SetParams				(1,false);	//.

	// codepath is the same, only the shaders differ
	// ***only pixel shaders differ***
	switch(C.iElement) 
	{
	case SE_R2_NORMAL_HQ: 	// deffer
		uber_deffer		(C,true,"base","base",true);
		break;
	case SE_R2_NORMAL_LQ: 	// deffer
		uber_deffer		(C,false,"base","base",true);
		break;
	case SE_R2_SHADOW:		// smap
		if (RImplementation.o.HW_smap)	C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE,TRUE,TRUE,FALSE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,220);
		else							C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	}
}
