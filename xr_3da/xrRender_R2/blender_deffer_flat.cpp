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
			char				fname	[_MAX_PATH];
			strcpy				(fname,C.L_textures[0]); if (strext(fname)) *strext(fname)=0;
			strcat				(fname,"_bump.dds");
//			if (FS.exist("$game_textures$",	fname))
//			{
				// bump found
				C.r_Pass		("deffer_base_bump","deffer_base_bump",FALSE);
				C.r_Sampler		("s_base",C.L_textures[0]);
				C.r_Sampler		("s_encodeRG",	r2_float2RG,false,D3DTADDRESS_WRAP,D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT);
				C.r_Sampler		("s_encodeB",	r2_float2B,	false,D3DTADDRESS_WRAP,D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT);
				C.r_Sampler		("s_ncm",		r2_ncm,		false,D3DTADDRESS_CLAMP,D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT);
				if (ps_r2_ls_flags.test(R2FLAG_BUMP_AF))	C.r_Sampler		("s_bump",fname,false,D3DTADDRESS_WRAP,D3DTEXF_ANISOTROPIC);
				else										C.r_Sampler		("s_bump",fname);
				C.r_End			();
/*			} else {
				// flat
				C.r_Pass		("deffer_base_flat","deffer_base_flat",FALSE);
				C.r_Sampler		("s_base",C.L_textures[0]);
				C.r_End			();
			} */
		} 
		break;
	case 1:		// smap-direct
		if (RImplementation.b_nv3x)	C.r_Pass			("shadow_direct_base","shadow_direct_base",FALSE,TRUE,TRUE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE);
		else						C.r_Pass			("shadow_direct_base","shadow_direct_base",FALSE);
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
		if (RImplementation.b_nv3x)	C.r_Pass			("shadow_spot_base","shadow_direct_base",FALSE,TRUE,TRUE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE);
		else						C.r_Pass			("shadow_spot_base","shadow_direct_base",FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	}
}
