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
void	CBlender_default::Load(	IReader& fs, u16 version )
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
		if (C.L_textures.size()<2)	Debug.fatal	("Not enought textures for shader, base tex: %s",C.L_textures[0]);
		switch (C.iElement)
		{
		case SE_R1_NORMAL_HQ:
		case SE_R1_NORMAL_LQ:
			// Level view
			if (C.bDetail)
			{
				C.r_Pass	("lmap_dt","lmap_dt",TRUE);
				C.r_Sampler	("s_base",	C.L_textures[0]);
				C.r_Sampler	("s_lmap",	C.L_textures[1]);
				C.r_Sampler	("s_detail",C.detail_texture);
				C.r_End		();
			} else
			{
				C.r_Pass	("lmap","lmap",TRUE);
				C.r_Sampler	("s_base",C.L_textures[0]);
				C.r_Sampler	("s_lmap",C.L_textures[1]);
				C.r_End		();
			}
			break;
		case SE_R1_LPOINT:
			break;
		case SE_R1_LSPOT:
			C.r_Pass		("lmap_spot","add_spot",FALSE,TRUE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE,TRUE);
			C.r_Sampler		("s_base",	C.L_textures[0]);
			C.r_Sampler_clf	("s_lmap",	"internal\\internal_light_att",		true);
			C.r_Sampler_clf	("s_att",	"internal\\internal_light_attclip"		);
			C.r_End			();
			break;
		case SE_R1_LMODELS:
			// Lighting only, not use alpha-channel
			C.r_Pass		("lmap_l","lmap_l",FALSE);
			C.r_Sampler		("s_base",C.L_textures[0]);
			C.r_Sampler		("s_lmap",C.L_textures[1]);
			C.r_End			();
			break;
		}
	}
}
