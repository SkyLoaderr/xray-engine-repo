// BlenderDefault.cpp: implementation of the CBlender_LmBmmD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "blender_LmBmmD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_LmBmmD::CBlender_LmBmmD	()
{
	description.CLS		= B_LmBmmD;
	strcpy				(oT2_Name,	"$null");
	strcpy				(oT2_xform,	"$null");
}

CBlender_LmBmmD::~CBlender_LmBmmD	()
{
	
}

void	CBlender_LmBmmD::Save		(CFS_Base& FS )
{
	CBlender::Save	(FS);
	xrPWRITE_MARKER	(FS,"Detail map");
	xrPWRITE_PROP	(FS,"Name",				xrPID_TEXTURE,	oT2_Name);
	xrPWRITE_PROP	(FS,"Transform",		xrPID_MATRIX,	oT2_xform);
}
void	CBlender_LmBmmD::Load		(CStream& FS, WORD version )
{
	CBlender::Load	(FS,version);
	xrPREAD_MARKER	(FS);
	xrPREAD_PROP	(FS,xrPID_TEXTURE,	oT2_Name);
	xrPREAD_PROP	(FS,xrPID_MATRIX,	oT2_xform);
}

void	CBlender_LmBmmD::Compile	(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	if (C.bEditor)	{
		C.RS.PassBegin		();
		{
			C.RS.PassSET_ZB		(TRUE,TRUE);
			C.RS.PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
			C.RS.PassSET_LightFog	(TRUE,TRUE);
			
			// Stage1 - Base texture
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
				C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				C.RS.Stage_Texture		(oT_Name,		C.L_textures		);
				C.RS.Stage_Matrix		(oT_xform,		C.L_matrices,		0);
				C.RS.Stage_Constant		("$null",		C.L_constants		);
			}
			C.RS.StageEnd			();
			
			// Stage2 - Second texture
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
				C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
				C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_CURRENT);
				C.RS.Stage_Texture		(oT2_Name,		C.L_textures		);
				C.RS.Stage_Matrix		(oT2_xform,		C.L_matrices,		0);
				C.RS.Stage_Constant		("$null",		C.L_constants		);
			}
			C.RS.StageEnd			();
		}
		C.RS.PassEnd			();
	} else {
		switch (HW.Caps.pixel.dwStages)
		{
		case 2:		// Geforce1/2/MX
			C.RS.PassBegin		();
			{
				C.RS.PassSET_ZB			(TRUE,TRUE);
				C.RS.PassSET_Blend		(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
				C.RS.PassSET_LightFog	(FALSE,TRUE);
				
				// Stage0 - Lightmap
				C.RS.StageBegin		();
				{
					C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
					C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.RS.Stage_Texture		("$base1",		C.L_textures	);
					C.RS.Stage_Matrix		("$null",		C.L_matrices,	1);
					C.RS.Stage_Constant		("$null",		C.L_constants	);
				}
				C.RS.StageEnd			();
				
				// Stage1 - Base texture
				C.RS.StageBegin		();
				{
					C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
					C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
					C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
					C.RS.Stage_Texture		(oT_Name,		C.L_textures	);
					C.RS.Stage_Matrix		(oT_xform,		C.L_matrices,	0);
					C.RS.Stage_Constant		("$null",		C.L_constants	);
				}
				C.RS.StageEnd			();
			}
			C.RS.PassEnd			();
			
			// Pass2 - Detail
			C.RS.PassBegin		();
			{
				C.RS.PassSET_ZB			(TRUE,FALSE);
				C.RS.PassSET_Blend		(TRUE,D3DBLEND_DESTCOLOR,D3DBLEND_SRCCOLOR,	FALSE,0);
				C.RS.PassSET_LightFog	(FALSE,TRUE);
				
				// Stage0 - Detail
				C.RS.StageBegin		();
				{
					C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
					C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.RS.Stage_Texture		(oT2_Name,		C.L_textures	);
					C.RS.Stage_Matrix		(oT2_xform,		C.L_matrices,	0);
					C.RS.Stage_Constant		("$null",		C.L_constants	);
				}
				C.RS.StageEnd			();
			}
			C.RS.PassEnd			();
			break;
		case 3:		// Kyro, Radeon, Radeon2, Geforce3/4
		default:
			C.RS.PassBegin		();
			{
				C.RS.PassSET_ZB			(TRUE,TRUE);
				C.RS.PassSET_Blend		(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,FALSE,0);
				C.RS.PassSET_LightFog	(FALSE,TRUE);
				
				// Stage0 - Lightmap
				C.RS.StageBegin		();
				{
					C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
					C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.RS.Stage_Texture		("$base1",		C.L_textures	);
					C.RS.Stage_Matrix		("$null",		C.L_matrices,	1);
					C.RS.Stage_Constant		("$null",		C.L_constants	);
				}
				C.RS.StageEnd			();
				
				// Stage1 - Base texture
				C.RS.StageBegin		();
				{
					C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
					C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
					C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_CURRENT);
					C.RS.Stage_Texture		(oT_Name,		C.L_textures	);
					C.RS.Stage_Matrix		(oT_xform,		C.L_matrices,	0);
					C.RS.Stage_Constant		("$null",		C.L_constants	);
				}
				C.RS.StageEnd			();
				
				// Stage2 - Detail
				C.RS.StageBegin		();
				{
					C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
					C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
					C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_CURRENT);
					C.RS.Stage_Texture		(oT2_Name,		C.L_textures	);
					C.RS.Stage_Matrix		(oT2_xform,		C.L_matrices,	0);
					C.RS.Stage_Constant		("$null",		C.L_constants	);
				}
				C.RS.StageEnd			();
			}
			C.RS.PassEnd			();
			break;
		}
	}
}
