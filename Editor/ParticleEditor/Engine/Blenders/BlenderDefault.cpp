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
void	CBlender_default::Compile(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param, BOOL bEditor)
{
	CBlender::Compile		(RS,L_textures,L_constants,L_matrices,param,bEditor);
	if (bEditor)	{
		RS.PassBegin		();
		{
			RS.PassSET_ZB		(TRUE,TRUE);
			RS.PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,FALSE,0);
			RS.PassSET_LightFog	(TRUE,TRUE);
			
			// Stage1 - Base texture
			RS.StageBegin		();
			{
				RS.StageSET_Address	(D3DTADDRESS_WRAP);
				RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				RS.Stage_Texture	(oT_Name,	L_textures);
				RS.Stage_Matrix		(oT_xform,	L_matrices,	0);
				RS.Stage_Constant	("$null",	L_constants);
			}
			RS.StageEnd			();
		}
		RS.PassEnd			();
	} else {
		RS.PassBegin		();
		{
			RS.PassSET_ZB		(TRUE,TRUE);
			RS.PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,FALSE,0);
			RS.PassSET_LightFog	(FALSE,TRUE);
			
			// Stage0 - Lightmap
			RS.StageBegin		();
			{
				RS.StageSET_Address	(D3DTADDRESS_WRAP);
				RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				RS.Stage_Texture	("$base1",L_textures);
				RS.Stage_Matrix		("$null",L_matrices,1);
				RS.Stage_Constant	("$null",L_constants);
			}
			RS.StageEnd			();
			
			// Stage1 - Base texture
			RS.StageBegin		();
			{
				RS.StageSET_Address	(D3DTADDRESS_WRAP);
				RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
				RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
				RS.Stage_Texture	(oT_Name,	L_textures);
				RS.Stage_Matrix		(oT_xform,	L_matrices,	0);
				RS.Stage_Constant	("$null",	L_constants);
			}
			RS.StageEnd			();
		}
		RS.PassEnd			();
	}
}
