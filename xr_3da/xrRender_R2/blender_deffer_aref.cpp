#include "stdafx.h"
#pragma hdrstop

#include "Blender_deffer_aref.h"

CBlender_deffer_aref::CBlender_deffer_aref	()	{	description.CLS		= B_DEFAULT_AREF;	}
CBlender_deffer_aref::~CBlender_deffer_aref	()	{	}

void	CBlender_deffer_aref::Save	(	IWriter& FS )
{
	CBlender::Save	(FS);
}
void	CBlender_deffer_aref::Load	(	IReader& FS, WORD version )
{
	CBlender::Load	(FS,version);
}

void	CBlender_deffer_aref::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);

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
				C.r2_Pass			(r2v("r2_deffer_base_bump"),r2p("r2_deffer_base_aref_bump"));
				C.r2_Sampler		("s_base",C.L_textures[0]);
				C.r2_Sampler		("s_bump",fname);
				C.r2_End			();
			} else {
				// flat
				C.r2_Pass			(r2v("r2_deffer_base_flat"),r2p("r2_deffer_base_aref_flat"));
				C.r2_Sampler		("s_base",C.L_textures[0]);
				C.r2_End			();
			}
		}
		break;
	case 1:		// smap-direct
		if (RImplementation.b_nv3x)	C.r2_Pass	(r2v("r2_shadow_direct_base"),r2p("r2_shadow_direct_aref"),TRUE,TRUE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,220);
		else						C.r2_Pass	(r2v("r2_shadow_direct_base"),r2p("r2_shadow_direct_aref"));
		C.r2_Sampler		("s_base",C.L_textures[0]);
		C.r2_End			();
		break;
	case 2:		// smap-point
		C.r2_Pass			(r2v("r2_shadow_point_base"),r2p("r2_shadow_point_base"));
		C.r2_Sampler		("s_base",			C.L_textures[0]);
		C.r2_Constant		("light_position",	&RImplementation.Binders.l_position);
		C.r2_End			();
		break;
	case 3:		// smap-spot
		if (RImplementation.b_nv3x)	C.r2_Pass	(r2v("r2_shadow_spot_base"),r2p("r2_shadow_direct_aref"),TRUE,TRUE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,220);
		else						C.r2_Pass	(r2v("r2_shadow_spot_base"),r2p("r2_shadow_direct_aref"));
		C.r2_Sampler		("s_base",C.L_textures[0]);
		C.r2_End			();
		break;
	}
}
