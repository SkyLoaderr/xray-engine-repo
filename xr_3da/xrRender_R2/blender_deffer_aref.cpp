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
			char				fname	[_MAX_PATH];
			strcpy				(fname,C.L_textures[0]); if (strext(fname)) *strext(fname)=0;
			strcat				(fname,"_bump.dds");
//			if (FS.exist("$game_textures$",	fname))
//			{
				// bump found
				C.r_Pass		("deffer_base_bump","deffer_base_aref_bump",FALSE);
				C.r_Sampler		("s_base",C.L_textures[0]);
				C.r_Sampler		("s_encodeRG",	r2_float2RG,false,D3DTADDRESS_WRAP, D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT);
				C.r_Sampler		("s_encodeB",	r2_float2B,	false,D3DTADDRESS_WRAP, D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT);
				C.r_Sampler		("s_ncm",		r2_ncm,		false,D3DTADDRESS_CLAMP,D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT);
				if (ps_r2_ls_flags.test(R2FLAG_BUMP_AF))	C.r_Sampler		("s_bump",fname,false,D3DTADDRESS_WRAP,D3DTEXF_ANISOTROPIC);
				else										C.r_Sampler		("s_bump",fname);
				C.r_End			();
/*			} else {
				// flat
				C.r_Pass		("deffer_base_flat","deffer_base_aref_flat",FALSE);
				C.r_Sampler		("s_base",C.L_textures[0]);
				C.r_End			();
			} */
		}
		break;
	case 1:		// smap-direct
		if (RImplementation.b_nv3x)	C.r_Pass	("shadow_direct_aref","shadow_direct_aref",FALSE,TRUE,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,TRUE,220);
		else						C.r_Pass	("shadow_direct_aref","shadow_direct_aref",FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	case 2:		// smap-point
		C.r_Pass		("shadow_point_base","shadow_point_base",FALSE);
		C.r_Sampler		("s_base",			C.L_textures[0]);
		C.r_Constant	("Ldynamic_pos",	&RImplementation.Binders.l_position);
		C.r_End			();
		break;
	case 3:		// smap-spot
		if (RImplementation.b_nv3x)	C.r_Pass	("shadow_direct_aref","shadow_direct_aref",FALSE,TRUE,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,TRUE,220);
		else						C.r_Pass	("shadow_direct_aref","shadow_direct_aref",FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	}
}
