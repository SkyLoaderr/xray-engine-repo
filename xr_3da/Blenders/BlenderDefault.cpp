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
void	CBlender_default::Save(	CFS_Base& FS )
{
	CBlender::Save	(FS);
}
void	CBlender_default::Load(	CStream& FS, WORD version )
{
	CBlender::Load	(FS,version);
}
void	CBlender_default::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	if (C.bEditor)	{
		C.RS.PassBegin		();
		{
			C.RS.PassSET_ZB			(TRUE,TRUE);
			C.RS.PassSET_Blend		(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,FALSE,0);
			C.RS.PassSET_LightFog	(TRUE,TRUE);
			
			// Stage1 - Base texture
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
				C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				C.RS.Stage_Texture		(oT_Name,	C.L_textures);
				C.RS.Stage_Matrix		(oT_xform,	C.L_matrices,	0);
				C.RS.Stage_Constant		("$null",	C.L_constants);
			}
			C.RS.StageEnd			();
		}
		C.RS.PassEnd			();
	} else {
		if (C.bLighting)	
		{
			// Lighting only
			C.RS.PassBegin		();
			{
				C.RS.PassSET_ZB			(TRUE,TRUE);
				C.RS.PassSET_Blend_SET	();
				C.RS.PassSET_LightFog	(FALSE,FALSE);
				
				// Stage0 - Lightmap
				C.RS.StageBegin		();
				{
					C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
					C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.RS.Stage_Texture		("$base1",	C.L_textures);
					C.RS.Stage_Matrix		("$null",	C.L_matrices,	1);
					C.RS.Stage_Constant		("$null",	C.L_constants);
				}
				C.RS.StageEnd			();
			}
			C.RS.PassEnd			();
		} else {
			// Level view
			C.RS.PassBegin		();
			{
				C.RS.PassSET_ZB			(TRUE,TRUE);
				C.RS.PassSET_Blend_SET	();
				C.RS.PassSET_LightFog	(FALSE,TRUE);
				
				// Stage0 - Lightmap
				C.RS.StageBegin		();
				{
					C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
					C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.RS.Stage_Texture		("$base1",	C.L_textures);
					C.RS.Stage_Matrix		("$null",	C.L_matrices,	1);
					C.RS.Stage_Constant		("$null",	C.L_constants);
				}
				C.RS.StageEnd			();
				
				// Stage1 - Base texture
				C.RS.StageBegin		();
				{
					C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
					C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
					C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
					C.RS.Stage_Texture		(oT_Name,	C.L_textures);
					C.RS.Stage_Matrix		(oT_xform,	C.L_matrices,	0);
					C.RS.Stage_Constant		("$null",	C.L_constants);
				}
				C.RS.StageEnd			();
			}
			C.RS.PassEnd			();
		}
	}
}
