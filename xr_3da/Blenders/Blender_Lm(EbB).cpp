// BlenderDefault.cpp: implementation of the CBlender_LmEbB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "blender_Lm(EbB).h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_LmEbB::CBlender_LmEbB	()
{
	description.CLS		= B_LmEbB;
	strcpy				(oT2_Name,	"$null");
	strcpy				(oT2_xform,	"$null");
}

CBlender_LmEbB::~CBlender_LmEbB	()
{
	
}

void	CBlender_LmEbB::Save(	CFS_Base& FS )
{
	CBlender::Save	(FS);
	xrPWRITE_MARKER	(FS,"Environment map");
	xrPWRITE_PROP	(FS,"Name",				xrPID_TEXTURE,	oT2_Name);
	xrPWRITE_PROP	(FS,"Transform",		xrPID_MATRIX,	oT2_xform);
}

void	CBlender_LmEbB::Load(	CStream& FS, WORD version )
{
	CBlender::Load	(FS,version);
	xrPREAD_MARKER	(FS);
	xrPREAD_PROP	(FS,xrPID_TEXTURE,	oT2_Name);
	xrPREAD_PROP	(FS,xrPID_MATRIX,	oT2_xform);
}

void	CBlender_LmEbB::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	if (C.bEditor)	{
		// NO CONSTANT
		
		C.RS.PassBegin		();
		{
			C.RS.PassSET_ZB		(TRUE,TRUE);
			C.RS.PassSET_Blend_SET();
			C.RS.PassSET_LightFog	(TRUE,TRUE);
			
			// Stage1 - Env texture
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
				C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
				C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
				C.RS.Stage_Texture		(oT2_Name,		C.L_textures		);
				C.RS.Stage_Matrix		(oT2_xform,		C.L_matrices,		0);
				C.RS.Stage_Constant		("$null",		C.L_constants		);
			}
			C.RS.StageEnd			();
			
			// Stage2 - Base texture
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
				C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_BLENDTEXTUREALPHA,	D3DTA_CURRENT);
				C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_CURRENT);
				C.RS.Stage_Texture		(oT_Name,		C.L_textures		);
				C.RS.Stage_Matrix		(oT_xform,		C.L_matrices,		0);
				C.RS.Stage_Constant		("$null",		C.L_constants		);
			}
			C.RS.StageEnd			();

			// Stage3 - Lighting - should work on all 2tex hardware
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
				C.RS.StageSET_Color		(D3DTA_DIFFUSE,	  D3DTOP_MODULATE,			D3DTA_CURRENT);
				C.RS.StageSET_Alpha		(D3DTA_DIFFUSE,	  D3DTOP_SELECTARG2,		D3DTA_CURRENT);
				C.RS.Stage_Texture		("$null",		C.L_textures		);
				C.RS.Stage_Matrix		("$null",		C.L_matrices,		0);
				C.RS.Stage_Constant		("$null",		C.L_constants		);
			}
			C.RS.StageEnd			();
		}
		C.RS.PassEnd			();
	} else {
		switch (HW.Caps.pixel.dwStages)
		{
		case 2:		// Geforce1/2/MX
			compile_2	(C);
			break;
		case 3:		// Kyro, Radeon, Radeon2, Geforce3/4
		default:
			compile_3	(C);
			break;
		}
	}
}

//
void CBlender_LmEbB::compile_2	(CBlender_Compile& C)
{
	// Pass1 - Lighting
	C.RS.PassBegin		();
	{
		C.RS.PassSET_ZB		(TRUE,TRUE);
		C.RS.PassSET_Blend_SET();
		C.RS.PassSET_LightFog	(FALSE,TRUE);
		
		// Stage0 - Detail
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
			C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
			C.RS.Stage_Texture		("$base1",		C.L_textures	);
			C.RS.Stage_Matrix		("$null",		C.L_matrices,	1);
			C.RS.Stage_Constant		("$null",		C.L_constants	);
		}
		C.RS.StageEnd			();
	}
	C.RS.PassEnd			();

	// Pass2 - (env^base)
	C.RS.PassBegin		();
	{
		C.RS.PassSET_ZB		(TRUE,FALSE);
		C.RS.PassSET_Blend_MUL();
		C.RS.PassSET_LightFog	(FALSE,TRUE);
		
		// Stage0 - Environment map
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
			C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
			C.RS.Stage_Texture		(oT2_Name,		C.L_textures	);
			C.RS.Stage_Matrix		(oT2_xform,		C.L_matrices,	0);
			C.RS.Stage_Constant		("$null",		C.L_constants	);
		}
		C.RS.StageEnd			();

		// Stage1 - Base map
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_BLENDTEXTUREALPHA,	D3DTA_CURRENT);
			C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_CURRENT);
			C.RS.Stage_Texture		(oT_Name,		C.L_textures	);
			C.RS.Stage_Matrix		(oT_xform,		C.L_matrices,	0);
			C.RS.Stage_Constant		("$null",		C.L_constants	);
		}
		C.RS.StageEnd			();
	}
	C.RS.PassEnd			();
}

//
void CBlender_LmEbB::compile_3	(CBlender_Compile& C)
{
	C.RS.PassBegin		();
	{
		C.RS.PassSET_ZB		(TRUE,TRUE);
		C.RS.PassSET_Blend_SET();
		C.RS.PassSET_LightFog	(FALSE,TRUE);
		
		// Stage0 - Environment map
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
			C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
			C.RS.Stage_Texture		(oT2_Name,		C.L_textures	);
			C.RS.Stage_Matrix		(oT2_xform,		C.L_matrices,	0);
			C.RS.Stage_Constant		("$null",		C.L_constants	);
		}
		C.RS.StageEnd			();
		
		// Stage1 - [^] Base map
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_BLENDTEXTUREALPHA,	D3DTA_CURRENT);
			C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_CURRENT);
			C.RS.Stage_Texture		(oT_Name,		C.L_textures	);
			C.RS.Stage_Matrix		(oT_xform,		C.L_matrices,	0);
			C.RS.Stage_Constant		("$null",		C.L_constants	);
		}
		C.RS.StageEnd			();
		
		// Stage2 - [*] Lightmap
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,			D3DTA_CURRENT);
			C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,		D3DTA_CURRENT);
			C.RS.Stage_Texture		("$base1",		C.L_textures	);
			C.RS.Stage_Matrix		("$null",		C.L_matrices,	1);
			C.RS.Stage_Constant		("$null",		C.L_constants	);
		}
		C.RS.StageEnd			();
	}
	C.RS.PassEnd			();
}
