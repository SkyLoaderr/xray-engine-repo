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

void	CBlender_LmEbB::Save(	IWriter& fs )
{
	CBlender::Save	(fs);
	xrPWRITE_MARKER	(fs,"Environment map");
	xrPWRITE_PROP	(fs,"Name",				xrPID_TEXTURE,	oT2_Name);
	xrPWRITE_PROP	(fs,"Transform",		xrPID_MATRIX,	oT2_xform);
}

void	CBlender_LmEbB::Load(	IReader& fs, WORD version )
{
	CBlender::Load	(fs,version);
	xrPREAD_MARKER	(fs);
	xrPREAD_PROP	(fs,xrPID_TEXTURE,	oT2_Name);
	xrPREAD_PROP	(fs,xrPID_MATRIX,	oT2_xform);
}

void	CBlender_LmEbB::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	if (C.bEditor)	{
		C.PassBegin		();
		{
			C.PassSET_ZB		(TRUE,TRUE);
			C.PassSET_Blend_SET	();
			C.PassSET_LightFog	(TRUE,TRUE);
			
			// Stage1 - Env texture
			C.StageBegin		();
			C.StageSET_Address	(D3DTADDRESS_CLAMP);
			C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
			C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
			C.StageSET_TMC		(oT2_Name, oT2_xform, "$null", 0);
			C.StageEnd			();
			
			// Stage2 - Base texture
			C.StageBegin		();
			C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_BLENDTEXTUREALPHA,	D3DTA_CURRENT);
			C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_CURRENT);
			C.StageSET_TMC		(oT_Name, oT_xform, "$null", 0);
			C.StageEnd			();

			// Stage3 - Lighting - should work on all 2tex hardware
			C.StageBegin		();
			C.StageSET_Color	(D3DTA_DIFFUSE,	  D3DTOP_MODULATE,			D3DTA_CURRENT);
			C.StageSET_Alpha	(D3DTA_DIFFUSE,	  D3DTOP_SELECTARG2,		D3DTA_CURRENT);
			C.Stage_Texture		("$null"	);
			C.Stage_Matrix		("$null",	0);
			C.Stage_Constant	("$null"	);
			C.StageEnd			();
		}
		C.PassEnd			();
	} else {
		if (2==C.iElement)	
		{
			compile_L	(C);
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
}

//
void CBlender_LmEbB::compile_2	(CBlender_Compile& C)
{
	// Pass1 - Lighting
	C.PassBegin		();
	{
		C.PassSET_ZB			(TRUE,TRUE);
		C.PassSET_Blend_SET		();
		C.PassSET_LightFog		(FALSE,TRUE);
		
		// Stage0 - Detail
		C.StageBegin			();
		C.StageTemplate_LMAP0	();
		C.StageEnd				();
	}
	C.PassEnd			();

	// Pass2 - (env^base)
	C.PassBegin		();
	{
		C.PassSET_ZB			(TRUE,FALSE);
		C.PassSET_Blend_MUL2X	();
		C.PassSET_LightFog		(FALSE,TRUE);
		
		// Stage0 - Environment map
		C.StageBegin			();
		C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
		C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
		C.StageSET_TMC			(oT2_Name, oT2_xform, "$null", 0);
		C.StageEnd				();

		// Stage1 - Base map
		C.StageBegin			();
		C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_BLENDTEXTUREALPHA,	D3DTA_CURRENT);
		C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_CURRENT);
		C.StageSET_TMC			(oT_Name, oT_xform, "$null", 0);
		C.StageEnd				();
	}
	C.PassEnd			();
}

//
void CBlender_LmEbB::compile_3	(CBlender_Compile& C)
{
	C.PassBegin		();
	{
		C.PassSET_ZB			(TRUE,TRUE);
		C.PassSET_Blend_SET		();
		C.PassSET_LightFog		(FALSE,TRUE);
		
		// Stage0 - Environment map
		C.StageBegin			();
		C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
		C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
		C.StageSET_TMC			(oT2_Name, oT2_xform, "$null", 0);
		C.StageEnd				();
		
		// Stage1 - [^] Base map
		C.StageBegin			();
		C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_BLENDTEXTUREALPHA,	D3DTA_CURRENT);
		C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_CURRENT);
		C.StageSET_TMC			(oT_Name, oT_xform, "$null", 0);
		C.StageEnd				();
		
		// Stage2 - [*] Lightmap
		C.StageBegin			();
		C.StageSET_Address		(D3DTADDRESS_CLAMP);
		C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,		D3DTA_CURRENT);
		C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,		D3DTA_CURRENT);
		C.Stage_Texture			("$base1"		);
		C.Stage_Matrix			("$null",		1);
		C.Stage_Constant		("$null"		);
		C.StageEnd			();
	}
	C.PassEnd			();
}
//
void CBlender_LmEbB::compile_L	(CBlender_Compile& C)
{
	// Pass1 - Lighting
	C.PassBegin		();
	{
		C.PassSET_ZB			(TRUE,TRUE);
		C.PassSET_Blend_SET		();
		C.PassSET_LightFog		(FALSE,FALSE);
		
		// Stage0 - Detail
		C.StageBegin			();
		C.StageTemplate_LMAP0	();
		C.StageEnd				();
	}
	C.PassEnd			();
}
