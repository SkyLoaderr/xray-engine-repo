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
	description.CLS			= B_BmmD;
	description.version		= 2;
}

CBlender_BmmD::~CBlender_BmmD	()
{
	
}

void	CBlender_BmmD::Save		(CFS_Base& FS )
{
	CBlender::Save	(FS);
}

void	CBlender_BmmD::Load		(CStream& FS, WORD version )
{
	CBlender::Load	(FS,version);

	string64		skip;
	switch(version) {
	case 0:
	case 1:
		xrPREAD_MARKER	(FS);
		xrPREAD_PROP	(FS,xrPID_TEXTURE,	skip);
		xrPREAD_PROP	(FS,xrPID_MATRIX,	skip);
		break;
	case 2:
	default:
		break;
	}
}

void	CBlender_BmmD::Compile	(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	if (C.bEditor)	{
		C.PassBegin		();
		{
			C.PassSET_ZB		(TRUE,TRUE);
			C.PassSET_Blend_SET	();
			C.PassSET_LightFog	(TRUE,TRUE);
			
			// Stage1 - Base texture
			C.StageBegin		();
			C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			C.StageSET_TMC		(oT_Name,oT_xform,"$null",0);
			C.StageEnd			();
		}
		C.PassEnd			();
	} else {
		if (C.bLighting)	
		{
			C.PassBegin		();
			{
				C.PassSET_ZB		(TRUE,TRUE);
				C.PassSET_Blend_SET	();
				C.PassSET_LightFog	(FALSE,TRUE);
				
				// Stage1 - Base texture
				C.StageBegin		();
				C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				C.StageSET_TMC		(oT_Name,oT_xform,"$null",0);
				C.StageEnd			();
			}
			C.PassEnd			();
		} else {
			C.PassBegin		();
			{
				C.PassSET_ZB		(TRUE,TRUE);
				C.PassSET_Blend_SET	();
				C.PassSET_LightFog	(FALSE,TRUE);
				
				// Stage1 - Base texture
				C.StageBegin		();
				C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				C.StageSET_TMC		(oT_Name,oT_xform,"$null",0);
				C.StageEnd			();
			}
			C.PassEnd			();
		}
	}
}
