// Blender_Vertex.cpp: implementation of the CBlender_LIGHT class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Blender_light.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_LIGHT::CBlender_LIGHT()
{
	description.CLS		= B_LIGHT;
}

CBlender_LIGHT::~CBlender_LIGHT()
{

}

void	CBlender_LIGHT::Save	( CFS_Base& FS	)
{
	CBlender::Save	(FS);
}

void	CBlender_LIGHT::Load	( CStream& FS, WORD version	)
{
	CBlender::Load	(FS,version);
}

void CBlender_LIGHT::Compile	(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	C.RS.PassBegin		();
	{
		C.RS.PassSET_ZB			(TRUE,TRUE);
		C.RS.PassSET_Blend		(TRUE,D3DBLEND_ONE,D3DBLEND_ONE,	TRUE,0);
		C.RS.PassSET_LightFog	(FALSE,FALSE);
		
		// Stage0 - 2D map
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_CLAMP);
			C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			C.RS.Stage_Texture		("$base0",	C.L_textures);
			C.RS.Stage_Matrix		("$null",	C.L_matrices,	0);
			C.RS.Stage_Constant		("$null",	C.L_constants);
		}
		C.RS.StageEnd			();

		// Stage1 - 1D map
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_CLAMP);
			C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
			C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
			C.RS.Stage_Texture		("$base1",	C.L_textures);
			C.RS.Stage_Matrix		("$null",	C.L_matrices,	1);
			C.RS.Stage_Constant		("$null",	C.L_constants);
		}
		C.RS.StageEnd			();
	}
	C.RS.PassEnd			();
}
