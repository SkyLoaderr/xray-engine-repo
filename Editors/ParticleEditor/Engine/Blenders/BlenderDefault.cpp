// BlenderDefault.cpp: implementation of the CBlender_default class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "BlenderDefault.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_default::CBlender_default	()
{
	description.CLS		= B_DEFAULT;
}

CBlender_default::~CBlender_default	()
{

}
void	CBlender_default::Save(	IWriter& fs )
{
	IBlender::Save	(fs);
}
void	CBlender_default::Load(	IReader& fs, WORD version )
{
	IBlender::Load	(fs,version);
}
void	CBlender_default::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);
	if (C.bEditor)	{
		C.PassBegin		();
		{
			C.PassSET_ZB			(TRUE,TRUE);
			C.PassSET_Blend			(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,FALSE,0);
			C.PassSET_LightFog		(TRUE,TRUE);
			
			// Stage1 - Base texture
			C.StageBegin			();
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			C.StageSET_TMC			(oT_Name,oT_xform,"$null",0);
			C.StageEnd				();
		}
		C.PassEnd			();
	} else {
		if (2==C.iElement)	
		{
			// Lighting only
			C.r_Pass	("r1_default_l","r1_default_l",FALSE);
			C.r_Sampler	("s_base",C.L_textures[0]);
			C.r_Sampler	("s_lmap",C.L_textures[1]);
			C.r_End		();
		} else {
			// Level view
			if (C.bDetail)
			{
				C.r_Pass	("r1_default_dt","r1_default_dt",TRUE);
				C.r_Sampler	("s_base",	C.L_textures[0]);
				C.r_Sampler	("s_lmap",	C.L_textures[1]);
				C.r_Sampler	("s_detail",C.detail_texture);
				C.r_End		();
			} else
			{
				C.r_Pass	("r1_default","r1_default",TRUE);
				C.r_Sampler	("s_base",C.L_textures[0]);
				C.r_Sampler	("s_lmap",C.L_textures[1]);
				C.r_End		();
			}
		}
	}
}
