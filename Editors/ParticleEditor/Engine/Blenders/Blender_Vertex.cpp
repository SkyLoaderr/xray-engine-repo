// Blender_Vertex.cpp: implementation of the CBlender_Vertex class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Blender_Vertex.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Vertex::CBlender_Vertex()
{
	description.CLS		= B_VERT;
}

CBlender_Vertex::~CBlender_Vertex()
{
	
}

void	CBlender_Vertex::Save	( IWriter& fs	)
{
	IBlender::Save	(fs);
}

void	CBlender_Vertex::Load	( IReader& fs, u16 version	)
{
	IBlender::Load	(fs,version);
}

void CBlender_Vertex::Compile	(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	if (C.bEditor)
	{
		// Editor shader
		C.PassBegin		();
		{
			C.PassSET_ZB			(TRUE,TRUE);
			C.PassSET_Blend			(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
			C.PassSET_LightFog		(TRUE,TRUE);
			
			// Stage0 - Base texture
			C.StageBegin			();
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,	D3DTA_DIFFUSE);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,	D3DTA_DIFFUSE);
			C.Stage_Texture			(oT_Name);
			C.Stage_Matrix			(oT_xform,0);
			C.Stage_Constant		("$null");
			C.StageEnd				();
		}
		C.PassEnd			();
	} else {
		if (2==C.iElement)	
		{
			// Lighting only
			C.r_Pass	("r1_vert_l","r1_vert_l",FALSE);
			C.r_Sampler	("s_base",C.L_textures[0]);
			C.r_End		();
		} else {
			// Level view
			if (C.bDetail)
			{
				C.r_Pass	("r1_vert_dt","r1_vert_dt",TRUE);
				C.r_Sampler	("s_base",	C.L_textures[0]);
				C.r_Sampler	("s_detail",C.detail_texture);
				C.r_End		();
			} else
			{
				C.r_Pass	("r1_vert","r1_vert",TRUE);
				C.r_Sampler	("s_base",C.L_textures[0]);
				C.r_End		();
			}
		}
	}
}
