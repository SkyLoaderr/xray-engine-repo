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

void CBlender_LIGHT::Compile	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param, BOOL bEditor)
{
	CBlender::Compile		(RS,L_textures,L_constants,L_matrices,param,bEditor);
	RS.PassBegin		();
	{
		RS.PassSET_ZB		(TRUE,TRUE);
		RS.PassSET_Blend	(TRUE,D3DBLEND_ONE,D3DBLEND_ONE,	TRUE,0);
		RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
		RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));

		// Stage0 - 2D map
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_CLAMP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			RS.Stage_Texture	("$base0",	L_textures);
			RS.Stage_Matrix		("$null",	L_matrices,	0);
			RS.Stage_Constant	("$null",	L_constants);
		}
		RS.StageEnd			();

		// Stage1 - 1D map
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_CLAMP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
			RS.Stage_Texture	("$base1",	L_textures);
			RS.Stage_Matrix		("$null",	L_matrices,	1);
			RS.Stage_Constant	("$null",	L_constants);
		}
		RS.StageEnd			();
	}
	RS.PassEnd			();
}
