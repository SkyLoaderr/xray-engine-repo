#include "stdafx.h"
#pragma hdrstop

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
		{
			string256		fname,fnameA,fnameB;
			strcpy			(fname,C.L_textures[0]); if (strext(fname)) *strext(fname)=0;
			strconcat		(fnameA,fname,"_bump");
			strconcat		(fnameB,"$user$",fname,"_bumpX");

			if (C.bDetail)	
			{
				string256		fNameDT;
				strconcat		(fNameDT,C.detail_texture,"_bump");

				// bump + Dbump
				C.r_Pass		("deffer_base_bump_d","deffer_base_aref_bump_d",FALSE);
				C.r_Sampler		("s_base",		C.L_textures[0],	false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
				C.r_Sampler		("s_ncm",		r2_ncm,				false,	D3DTADDRESS_CLAMP,	D3DTEXF_LINEAR,		D3DTEXF_NONE,	D3DTEXF_LINEAR);
				C.r_Sampler		("s_bumpX",		fnameB,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);	// should be before base bump
				C.r_Sampler		("s_bump",		fnameA,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
				C.r_Sampler		("s_bumpD",		fNameDT,			false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
				C.r_End			();
			} else {
				// bump only
				C.r_Pass		("deffer_base_bump","deffer_base_aref_bump",FALSE);
				C.r_Sampler		("s_base",		C.L_textures[0],	false, D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
				C.r_Sampler		("s_ncm",		r2_ncm,				false, D3DTADDRESS_CLAMP,	D3DTEXF_LINEAR,		D3DTEXF_NONE,	D3DTEXF_LINEAR);
				C.r_Sampler		("s_bumpX",		fnameB,				false, D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);	// should be before base bump
				C.r_Sampler		("s_bump",		fnameA,				false, D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
				C.r_End			();
			}
		}
		break;
	case 1:		// smap-direct
		if (RImplementation.b_HW_smap)	C.r_Pass	("shadow_direct_aref","shadow_direct_aref",FALSE,TRUE,TRUE,FALSE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,220);
		else							C.r_Pass	("shadow_direct_aref","shadow_direct_aref",FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	case 2:		// smap-point
		C.r_Pass		("shadow_point_base","shadow_point_aref",FALSE);
		C.r_Sampler		("s_base",			C.L_textures[0]);
		C.r_End			();
		break;
	case 3:		// smap-spot
		if (RImplementation.b_HW_smap)	C.r_Pass	("shadow_direct_aref","shadow_direct_aref",FALSE,TRUE,TRUE,FALSE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,220);
		else							C.r_Pass	("shadow_direct_aref","shadow_direct_aref",FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	}
}
