// BlenderDefault.cpp: implementation of the CBlender_complex2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "blender_complex2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_complex2::CBlender_complex2	()
{
	description.CLS		= B_DEFAULT;
	
}

CBlender_complex2::~CBlender_complex2	()
{

}

void	CBlender_complex2::Save(	CFS_Base& FS )
{
	CBlender::Save	(FS);
}
void	CBlender_complex2::Load(	CStream& FS )
{
	CBlender::Load	(FS);
}
void	CBlender_complex2::Compile(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param, BOOL bEditor)
{
	if (bEditor)	{
		RS.PassBegin		();
		{
			RS.PassSET_ZB		(TRUE,TRUE);
			RS.PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,FALSE,0);
			RS.R().SetRS		(D3DRS_LIGHTING,					BC(TRUE));
			RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
			
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
			RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
			RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
			
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
