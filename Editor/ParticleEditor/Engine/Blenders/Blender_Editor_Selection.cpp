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

void	CBlender_Editor_Selection::Save	( CFS_Base& FS	)
{
	CBlender::Save	(FS);
	xrPWRITE_PROP	(FS,"TFactor",	xrPID_CONSTANT, oT_Factor);
}

void	CBlender_Editor_Selection::Load	( CStream& FS, WORD version	)
{
	CBlender::Load	(FS,version);
	xrPREAD_PROP	(FS,xrPID_CONSTANT,	oT_Factor);
}

void	CBlender_Editor_Selection::Compile	(CBlender_Compile& C)
{
	CBlender::Compile		(C);	
	C.RS.PassBegin		();
	{
		C.RS.PassSET_ZB			(TRUE,FALSE);
		C.RS.PassSET_Blend		(TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,	FALSE,0);
		C.RS.PassSET_LightFog	(FALSE,FALSE);
		
		// Stage0 - Base texture
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_CLAMP);
			C.RS.StageSET_Color		(D3DTA_TFACTOR,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			C.RS.StageSET_Alpha		(D3DTA_TFACTOR,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			C.RS.Stage_Texture		(oT_Name,	C.L_textures);
			C.RS.Stage_Matrix		(oT_xform,	C.L_matrices,	0);
			C.RS.Stage_Constant		("$null",	C.L_constants);
		}
		C.RS.StageEnd			();
	}
	C.RS.PassEnd			();
}
