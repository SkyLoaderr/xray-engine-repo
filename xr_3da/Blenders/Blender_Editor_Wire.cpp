#include "stdafx.h"
#pragma hdrstop

#include "Blender_Editor_Wire.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Editor_Wire::CBlender_Editor_Wire()
{
	description.CLS		= B_EDITOR_WIRE;
	strcpy				(oT_Factor,"$null");
}

CBlender_Editor_Wire::~CBlender_Editor_Wire()
{

}

void	CBlender_Editor_Wire::Save	( CFS_Base& FS	)
{
	CBlender::Save	(FS);
	xrPWRITE_PROP	(FS,"TFactor",	xrPID_CONSTANT, oT_Factor);
}

void	CBlender_Editor_Wire::Load	( CStream& FS, WORD version	)
{
	CBlender::Load	(FS,version);
	xrPREAD_PROP	(FS,xrPID_CONSTANT,	oT_Factor);
}

void CBlender_Editor_Wire::Compile	(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	C.PassBegin		();
	{
		C.PassSET_ZB		(TRUE,TRUE);
		C.PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
		C.PassSET_LightFog	(FALSE,FALSE);

		// Stage0 - Base texture
		C.StageBegin		();
		{
			C.StageSET_Address	(D3DTADDRESS_WRAP);
			C.StageSET_Color	(D3DTA_DIFFUSE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			C.StageSET_Alpha	(D3DTA_DIFFUSE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			C.Stage_Texture		("$null",	C.L_textures);
			C.Stage_Matrix		("$null",	C.L_matrices,	0);
			C.Stage_Constant	("$null",	C.L_constants);
//			C.Stage_Constant	("$base0",	"$user$wire");
		}
		C.StageEnd			();
	}
	C.PassEnd			();
}
