#include "stdafx.h"
#pragma hdrstop

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
		{
			string256		fname,fnameA,fnameB;
			strcpy			(fname,C.L_textures[0]); if (strext(fname)) *strext(fname)=0;
			strconcat		(fnameA,fname,"_bump");
			strconcat		(fnameB,"$user$",fname,"_bumpX");

			if (C.bDetail_Bump)	
			{
				string256		fNameDT;
				strconcat		(fNameDT,C.detail_texture,"_bump");

				// bump + Dbump
				C.r_Pass		("deffer_base_bump_d","deffer_base_bump_d",FALSE);
				C.r_Sampler		("s_base",		C.L_textures[0],	false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
				C.r_Sampler		("s_ncm",		r2_ncm,				false,	D3DTADDRESS_CLAMP,	D3DTEXF_LINEAR,		D3DTEXF_NONE,	D3DTEXF_LINEAR);
				C.r_Sampler		("s_bumpX",		fnameB,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);	// should be before base bump
				C.r_Sampler		("s_bump",		fnameA,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
				C.r_Sampler		("s_bumpD",		fNameDT,			false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
				C.r_End			();
			} else {
				// bump only
				C.r_Pass		("deffer_base_bump","deffer_base_bump",FALSE);
				C.r_Sampler		("s_base",		C.L_textures[0],	false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
				C.r_Sampler		("s_ncm",		r2_ncm,				false,	D3DTADDRESS_CLAMP,	D3DTEXF_LINEAR,		D3DTEXF_NONE,	D3DTEXF_LINEAR);
				C.r_Sampler		("s_bumpX",		fnameB,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);	// should be before base bump
				C.r_Sampler		("s_bump",		fnameA,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
				C.r_End			();
			}
		}
		break;
	case 1:		// smap-direct
		if (RImplementation.b_HW_smap)	C.r_Pass	("shadow_direct_base","dumb",	FALSE,TRUE,TRUE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE);
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
		if (RImplementation.b_HW_smap)	C.r_Pass	("shadow_direct_base","dumb",FALSE,TRUE,TRUE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE);
		else							C.r_Pass	("shadow_direct_base","shadow_direct_base",FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	case 4: 	// deffer-emap
		{
			string256		fname,fnameA,fnameB;
			strcpy			(fname,C.L_textures[0]); if (strext(fname)) *strext(fname)=0;
			strconcat		(fnameA,fname,"_bump");
			strconcat		(fnameB,"$user$",fname,"_bumpX");

			if (C.bDetail_Bump)	
			{
				string256		fNameDT;
				strconcat		(fNameDT,C.detail_texture,"_bump");

				// bump + Dbump
				C.r_Pass		("deffer_base_bump_d_e","deffer_base_bump_d_e",FALSE);
				C.r_Sampler		("s_base",		C.L_textures[0],	false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
				C.r_Sampler		("s_ncm",		r2_ncm,				false,	D3DTADDRESS_CLAMP,	D3DTEXF_LINEAR,		D3DTEXF_NONE,	D3DTEXF_LINEAR);
				C.r_Sampler		("s_bumpX",		fnameB,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);	// should be before base bump
				C.r_Sampler		("s_bump",		fnameA,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
				C.r_Sampler		("s_bumpD",		fNameDT,			false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
				C.r_End			();
			} else {
				// bump only
				C.r_Pass		("deffer_base_bump_e","deffer_base_bump_e",FALSE);
				C.r_Sampler		("s_base",		C.L_textures[0],	false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
				C.r_Sampler		("s_ncm",		r2_ncm,				false,	D3DTADDRESS_CLAMP,	D3DTEXF_LINEAR,		D3DTEXF_NONE,	D3DTEXF_LINEAR);
				C.r_Sampler		("s_bumpX",		fnameB,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);	// should be before base bump
				C.r_Sampler		("s_bump",		fnameA,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
				C.r_End			();
			}
		}
		break;
	}
}
