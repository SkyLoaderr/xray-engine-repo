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
		C.PassBegin		();
		{
			C.PassSET_ZB		(TRUE,TRUE);
			C.PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
			C.PassSET_LightFog	(TRUE,TRUE);
			
			// Stage1 - Base texture
			C.StageBegin		();
			{
				C.StageSET_Address	(D3DTADDRESS_WRAP);
				C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				C.Stage_Texture		(oT_Name,		C.L_textures		);
				C.Stage_Matrix		(oT_xform,		C.L_matrices,		0);
				C.Stage_Constant		("$null",		C.L_constants		);
			}
			C.StageEnd			();
			
			// Stage2 - Second texture
			C.StageBegin		();
			{
				C.StageSET_Address	(D3DTADDRESS_WRAP);
				C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
				C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_CURRENT);
				C.Stage_Texture		(oT2_Name,		C.L_textures		);
				C.Stage_Matrix		(oT2_xform,		C.L_matrices,		0);
				C.Stage_Constant		("$null",		C.L_constants		);
			}
			C.StageEnd			();
		}
		C.PassEnd			();
	} else {
		if (C.bLighting)	
		{
			C.PassBegin		();
			{
				C.PassSET_ZB			(TRUE,TRUE);
				C.PassSET_Blend_SET	();
				C.PassSET_LightFog	(FALSE,FALSE);
				
				// Stage0 - Lightmap
				C.StageBegin		();
				{
					C.StageSET_Address	(D3DTADDRESS_WRAP);
					C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.Stage_Texture		("$base1",		C.L_textures	);
					C.Stage_Matrix		("$null",		C.L_matrices,	1);
					C.Stage_Constant		("$null",		C.L_constants	);
				}
				C.StageEnd			();
			}
			C.PassEnd			();
		} else {
			switch (HW.Caps.pixel.dwStages)
			{
			case 2:		// Geforce1/2/MX
				C.PassBegin		();
				{
					C.PassSET_ZB			(TRUE,TRUE);
					C.PassSET_Blend_SET	();
					C.PassSET_LightFog	(FALSE,TRUE);
					
					// Stage0 - Lightmap
					C.StageBegin		();
					{
						C.StageSET_Address	(D3DTADDRESS_WRAP);
						C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
						C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
						C.Stage_Texture		("$base1",		C.L_textures	);
						C.Stage_Matrix		("$null",		C.L_matrices,	1);
						C.Stage_Constant		("$null",		C.L_constants	);
					}
					C.StageEnd			();
					
					// Stage1 - Base texture
					C.StageBegin		();
					{
						C.StageSET_Address	(D3DTADDRESS_WRAP);
						C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
						C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
						C.Stage_Texture		(oT_Name,		C.L_textures	);
						C.Stage_Matrix		(oT_xform,		C.L_matrices,	0);
						C.Stage_Constant		("$null",		C.L_constants	);
					}
					C.StageEnd			();
				}
				C.PassEnd			();
				
				// Pass2 - Detail
				C.PassBegin		();
				{
					C.PassSET_ZB			(TRUE,FALSE);
					C.PassSET_Blend_MUL2X();
					C.PassSET_LightFog	(FALSE,TRUE);
					
					// Stage0 - Detail
					C.StageBegin		();
					{
						C.StageSET_Address	(D3DTADDRESS_WRAP);
						C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
						C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
						C.Stage_Texture		(oT2_Name,		C.L_textures	);
						C.Stage_Matrix		(oT2_xform,		C.L_matrices,	0);
						C.Stage_Constant		("$null",		C.L_constants	);
					}
					C.StageEnd			();
				}
				C.PassEnd			();
				break;
			case 3:		// Kyro, Radeon, Radeon2, Geforce3/4
			default:
				C.PassBegin		();
				{
					C.PassSET_ZB			(TRUE,TRUE);
					C.PassSET_Blend_SET	();
					C.PassSET_LightFog	(FALSE,TRUE);
					
					// Stage0 - Lightmap
					C.StageBegin		();
					{
						C.StageSET_Address	(D3DTADDRESS_WRAP);
						C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
						C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
						C.Stage_Texture		("$base1",		C.L_textures	);
						C.Stage_Matrix		("$null",		C.L_matrices,	1);
						C.Stage_Constant		("$null",		C.L_constants	);
					}
					C.StageEnd			();
					
					// Stage1 - Base texture
					C.StageBegin		();
					{
						C.StageSET_Address	(D3DTADDRESS_WRAP);
						C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
						C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_CURRENT);
						C.Stage_Texture		(oT_Name,		C.L_textures	);
						C.Stage_Matrix		(oT_xform,		C.L_matrices,	0);
						C.Stage_Constant		("$null",		C.L_constants	);
					}
					C.StageEnd			();
					
					// Stage2 - Detail
					C.StageBegin		();
					{
						C.StageSET_Address	(D3DTADDRESS_WRAP);
						C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
						C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_CURRENT);
						C.Stage_Texture		(oT2_Name,		C.L_textures	);
						C.Stage_Matrix		(oT2_xform,		C.L_matrices,	0);
						C.Stage_Constant		("$null",		C.L_constants	);
					}
					C.StageEnd			();
				}
				C.PassEnd			();
				break;
			}
		}
	}
}
