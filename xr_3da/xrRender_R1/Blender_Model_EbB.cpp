// BlenderDefault.cpp: implementation of the CBlender_Model_EbB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "blender_Model_EbB.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Model_EbB::CBlender_Model_EbB	()
{
	description.CLS		= B_MODEL_EbB;
	strcpy				(oT2_Name,	"$null");
	strcpy				(oT2_xform,	"$null");
}

CBlender_Model_EbB::~CBlender_Model_EbB	()
{
	
}

void	CBlender_Model_EbB::Save(	IWriter& fs )
{
	IBlender::Save	(fs);
	xrPWRITE_MARKER	(fs,"Environment map");
	xrPWRITE_PROP	(fs,"Name",				xrPID_TEXTURE,	oT2_Name);
	xrPWRITE_PROP	(fs,"Transform",		xrPID_MATRIX,	oT2_xform);
}

void	CBlender_Model_EbB::Load(	IReader& fs, WORD version )
{
	IBlender::Load	(fs,version);
	xrPREAD_MARKER	(fs);
	xrPREAD_PROP	(fs,xrPID_TEXTURE,	oT2_Name);
	xrPREAD_PROP	(fs,xrPID_MATRIX,	oT2_xform);
}

void	CBlender_Model_EbB::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);
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
		C.PassBegin		();
		{
			C.PassSET_ZB		(TRUE,TRUE);
			C.PassSET_Blend_SET	();
			C.PassSET_LightFog	(TRUE,TRUE);
			
			switch (C.iElement)	
			{
			case 0:	// Highest LOD
				// Stage0 - projector
				C.StageBegin		();
				C.StageSET_TMC		("$user$projector", "$user$projector", "$null", 0	);
				C.StageEnd			();

				// Stage1 - Env texture
				C.StageBegin		();
				C.StageSET_Address	(D3DTADDRESS_CLAMP);
				C.StageSET_TMC		(oT2_Name, oT2_xform, "$null", 0);
				C.StageEnd			();

				// Stage2 - Base texture
				C.StageBegin		();
				C.StageSET_TMC		(oT_Name, oT_xform, "$null", 0);
				C.StageEnd			();

				C.PassSET_PS		("r1\\r1_model_env");
				break;
			case 1:	// Lowest LOD
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
				C.StageSET_Color	(D3DTA_DIFFUSE,	  D3DTOP_MODULATE2X,		D3DTA_CURRENT);
				C.StageSET_Alpha	(D3DTA_DIFFUSE,	  D3DTOP_SELECTARG2,		D3DTA_CURRENT);
				C.Stage_Texture		("$null"	);
				C.Stage_Matrix		("$null",	0);
				C.Stage_Constant	("$null"	);
				C.StageEnd			();
				break;
			}
		}
		C.PassEnd			();
	}
}
