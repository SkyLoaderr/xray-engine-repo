#include "stdafx.h"
#pragma hdrstop

#include "Blender_Editor_Selection.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Editor_Selection::CBlender_Editor_Selection()
{
	description.CLS		= B_EDITOR_SEL;
	strcpy				(oT_Factor,"$null");
}

CBlender_Editor_Selection::~CBlender_Editor_Selection()
{
	
}

void	CBlender_Editor_Selection::Save	( IWriter& FS	)
{
	CBlender::Save	(FS);
	xrPWRITE_PROP	(FS,"TFactor",	xrPID_CONSTANT, oT_Factor);
}

void	CBlender_Editor_Selection::Load	( IReader& FS, WORD version	)
{
	CBlender::Load	(FS,version);
	xrPREAD_PROP	(FS,xrPID_CONSTANT,	oT_Factor);
}

void	CBlender_Editor_Selection::Compile	(CBlender_Compile& C)
{
	CBlender::Compile		(C);	
	C.PassBegin		();
	{
		C.PassSET_ZB		(TRUE,FALSE);
		C.PassSET_Blend		(TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,	FALSE,0);
		C.PassSET_LightFog	(FALSE,FALSE);
		
		// Stage0 - Base texture
		C.StageBegin		();
		C.StageSET_Address	(D3DTADDRESS_CLAMP);
		C.StageSET_Color	(D3DTA_TFACTOR,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
		C.StageSET_Alpha	(D3DTA_TFACTOR,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
		C.Stage_Texture		(oT_Name	);
		C.Stage_Matrix		(oT_xform,	0);
		C.Stage_Constant	("$null"	);
		C.StageEnd			();
	}
	C.PassEnd			();
}
