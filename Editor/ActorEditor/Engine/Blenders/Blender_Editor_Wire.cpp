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

void	CBlender_Editor_Wire::Load	( CStream& FS	)
{
	CBlender::Load	(FS);
	xrPREAD_PROP	(FS,xrPID_CONSTANT,	oT_Factor);
}

void CBlender_Editor_Wire::Compile	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param, BOOL bEditor)
{
	RS.PassBegin		();
	{
		RS.PassSET_ZB		(TRUE,TRUE);
		RS.PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
		RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
		RS.R().SetRS		(D3DRS_FOGENABLE,					BC(FALSE));

		// Stage0 - Base texture
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_DIFFUSE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			RS.StageSET_Alpha	(D3DTA_DIFFUSE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			RS.Stage_Texture	(oT_Name,	L_textures);
			RS.Stage_Matrix		(oT_xform,	L_matrices,	0);
			RS.Stage_Constant	(oT_Factor,	L_constants);
		}
		RS.StageEnd			();
	}
	RS.PassEnd			();
}
