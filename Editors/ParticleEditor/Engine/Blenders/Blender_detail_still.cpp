// Blender_Vertex_aref.cpp: implementation of the CBlender_Detail_Still class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Blender_Detail_still.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Detail_Still::CBlender_Detail_Still()
{
	description.CLS		= B_DETAIL;
	description.version	= 0;
}

CBlender_Detail_Still::~CBlender_Detail_Still()
{

}

void	CBlender_Detail_Still::Save		(IWriter& fs )
{
	CBlender::Save		(fs);
	xrPWRITE_PROP		(fs,"Alpha-blend",	xrPID_BOOL,		oBlend);
}

void	CBlender_Detail_Still::Load		(IReader& fs, WORD version )
{
	CBlender::Load		(fs,version);
	xrPREAD_PROP		(fs,xrPID_BOOL,		oBlend);
}

void	CBlender_Detail_Still::Compile	(CBlender_Compile& C)
{
	CBlender::Compile	(C);
	
	if (C.bEditor)
	{
		C.PassBegin		();
		{
			C.PassSET_ZB		(TRUE,TRUE);
			if (oBlend.value)	C.PassSET_Blend_BLEND	(TRUE, 200);
			else				C.PassSET_Blend_SET		(TRUE, 200);
			C.PassSET_LightFog	(TRUE,TRUE);
			
			// Stage1 - Base texture
			C.StageBegin		();
			C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,	D3DTA_DIFFUSE);
			C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,	D3DTA_DIFFUSE);
			C.StageSET_TMC		(oT_Name,"$null","$null",0);
			C.StageEnd			();
		}
		C.PassEnd			();
	} else {
		switch (C.iElement)
		{
		case 0:
		case 1:
			C.PassBegin		();
			{
				C.PassSET_ZB		(TRUE,TRUE);
				if (oBlend.value)	C.PassSET_Blend_BLEND	(TRUE, 200);
				else				C.PassSET_Blend_SET		(TRUE, 200);
				C.PassSET_LightFog	(FALSE,FALSE);

				switch (C.iElement)	{
				case 0:	C.PassSET_VS("r1_detail_wave");		break;
				case 1:	C.PassSET_VS("r1_detail_still");	break;
				}

				// Stage1 - Base texture
				C.StageBegin		();
				C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_DIFFUSE);
				C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_DIFFUSE);
				C.StageSET_TMC		(oT_Name,"$null","$null",0);
				C.StageEnd			();
			}
			C.PassEnd			();
			break;
		case 2:
			C.PassBegin		();
			{
				C.PassSET_ZB		(TRUE,TRUE	);
				if (oBlend.value)	C.PassSET_Blend_BLEND	(TRUE, 200);
				else				C.PassSET_Blend_SET		(TRUE, 200);
				C.PassSET_LightFog	(FALSE,FALSE);
				C.PassSET_VS		("r1_detail_still");

				// Stage1 - Base texture
				C.StageBegin		();
				C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_DIFFUSE);
				C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				C.StageSET_TMC		(oT_Name,"$null","$null",0);
				C.StageEnd			();
			}
			C.PassEnd			();
			break;
		}
	}
}
