#include "stdafx.h"
#pragma hdrstop

#include "Blender_deffer_aref.h"

CBlender_deffer_aref::CBlender_deffer_aref	()	{	description.CLS		= B_DEFAULT_AREF;	}
CBlender_deffer_aref::~CBlender_deffer_aref	()	{	}

void	CBlender_deffer_aref::Save	(	IWriter& fs )
{
	IBlender::Save	(fs);
}
void	CBlender_deffer_aref::Load	(	IReader& fs, WORD version )
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
			if (FS.exist("$game_textures$",	fname))
			{
				// bump found
				C.r_Pass			(r2v("r2_deffer_base_bump"),r2p("r2_deffer_base_aref_bump"),FALSE);
				C.r_Sampler		("s_base",C.L_textures[0]);
				if (ps_r2_ls_flags.test(R2FLAG_BUMP_AF))	C.r_Sampler		("s_bump",fname,D3DTADDRESS_WRAP,D3DTEXF_ANISOTROPIC);
				else										C.r_Sampler		("s_bump",fname);
				C.r_End			();
			} else {
				// flat
				C.r_Pass			(r2v("r2_deffer_base_flat"),r2p("r2_deffer_base_aref_flat"),FALSE);
				C.r_Sampler		("s_base",C.L_textures[0]);
				C.r_End			();
			}
		}
		break;
	case 1:		// smap-direct
		if (RImplementation.b_nv3x)	C.r_Pass	(r2v("r2_shadow_direct_aref"),r2p("r2_shadow_direct_aref"),FALSE,TRUE,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,TRUE,220);
		else						C.r_Pass	(r2v("r2_shadow_direct_aref"),r2p("r2_shadow_direct_aref"),FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	case 2:		// smap-point
		C.r_Pass		(r2v("r2_shadow_point_base"),r2p("r2_shadow_point_base"),FALSE);
		C.r_Sampler		("s_base",			C.L_textures[0]);
		C.r_Constant	("light_position",	&RImplementation.Binders.l_position);
		C.r_End			();
		break;
	case 3:		// smap-spot
		if (RImplementation.b_nv3x)	C.r_Pass	(r2v("r2_shadow_direct_aref"),r2p("r2_shadow_direct_aref"),FALSE,TRUE,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,TRUE,220);
		else						C.r_Pass	(r2v("r2_shadow_direct_aref"),r2p("r2_shadow_direct_aref"),FALSE);
		C.r_Sampler		("s_base",C.L_textures[0]);
		C.r_End			();
		break;
	}
}
