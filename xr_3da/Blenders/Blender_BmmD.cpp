// BlenderDefault.cpp: implementation of the CBlender_BmmD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "blender_BmmD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_BmmD::CBlender_BmmD	()
{
	description.CLS		= B_BmmD;
	strcpy				(oT2_Name,	"$null");
	strcpy				(oT2_xform,	"$null");
}

CBlender_BmmD::~CBlender_BmmD	()
{
	
}

void	CBlender_BmmD::Save(	CFS_Base& FS )
{
	CBlender::Save	(FS);
	xrPWRITE_MARKER	(FS,"Detail map");
	xrPWRITE_PROP	(FS,"Name",				xrPID_TEXTURE,	oT2_Name);
	xrPWRITE_PROP	(FS,"Transform",		xrPID_MATRIX,	oT2_xform);
}

void	CBlender_BmmD::Load(	CStream& FS, WORD version )
{
	CBlender::Load	(FS,version);
	xrPREAD_MARKER	(FS);
	xrPREAD_PROP	(FS,xrPID_TEXTURE,	oT2_Name);
	xrPREAD_PROP	(FS,xrPID_MATRIX,	oT2_xform);
}

void	CBlender_BmmD::Compile(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param, BOOL bEditor)
{
	if (bEditor)	{
		RS.PassBegin		();
		{
			RS.PassSET_ZB		(TRUE,TRUE);
			RS.PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
			RS.R().SetRS		(D3DRS_LIGHTING,					BC(TRUE));
			RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
			
			// Stage1 - Base texture
			RS.StageBegin		();
			{
				RS.StageSET_Address	(D3DTADDRESS_WRAP);
				RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				RS.Stage_Texture	(oT_Name,		L_textures		);
				RS.Stage_Matrix		(oT_xform,		L_matrices,		0);
				RS.Stage_Constant	("$null",		L_constants		);
			}
			RS.StageEnd			();
			
			// Stage2 - Second texture
			RS.StageBegin		();
			{
				RS.StageSET_Address	(D3DTADDRESS_WRAP);
				RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
				RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_CURRENT);
				RS.Stage_Texture	(oT2_Name,		L_textures		);
				RS.Stage_Matrix		(oT2_xform,		L_matrices,		0);
				RS.Stage_Constant	("$null",		L_constants		);
			}
			RS.StageEnd			();
		}
		RS.PassEnd			();
	} else {
		RS.PassBegin		();
		{
			RS.PassSET_ZB		(TRUE,TRUE);
			RS.PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
			RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
			RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
			
			// Stage1 - Base texture
			RS.StageBegin		();
			{
				RS.StageSET_Address	(D3DTADDRESS_WRAP);
				RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				RS.Stage_Texture	(oT_Name,		L_textures		);
				RS.Stage_Matrix		(oT_xform,		L_matrices,		0);
				RS.Stage_Constant	("$null",		L_constants		);
			}
			RS.StageEnd			();
			
			// Stage2 - Second texture
			RS.StageBegin		();
			{
				RS.StageSET_Address	(D3DTADDRESS_WRAP);
				RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
				RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_CURRENT);
				RS.Stage_Texture	(oT2_Name,		L_textures		);
				RS.Stage_Matrix		(oT2_xform,		L_matrices,		0);
				RS.Stage_Constant	("$null",		L_constants		);
			}
			RS.StageEnd			();
		}
		RS.PassEnd			();
	}
}
