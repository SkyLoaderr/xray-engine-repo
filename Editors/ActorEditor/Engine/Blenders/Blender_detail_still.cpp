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
	description.CLS		= B_DETAIL_STILL;
	description.version	= 0;
}

CBlender_Detail_Still::~CBlender_Detail_Still()
{

}

void	CBlender_Detail_Still::Save		(CFS_Base& FS )
{
	CBlender::Save		(FS);
	xrPWRITE_PROP		(FS,"Alpha-blend",	xrPID_BOOL,		oBlend);
}

void	CBlender_Detail_Still::Load		(CStream& FS, WORD version )
{
	CBlender::Load		(FS,version);
	xrPREAD_PROP		(FS,xrPID_BOOL,		oBlend);
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
		if (C.bLighting)	
		{
			C.PassBegin		();
			{
				C.PassSET_ZB		(TRUE,TRUE	);
				if (oBlend.value)	C.PassSET_Blend_BLEND	(TRUE, 200);
				else				C.PassSET_Blend_SET		(TRUE, 200);
				C.PassSET_LightFog	(FALSE,FALSE);
				switch (C.iLOD)
				{
				case 0:	C.PassSET_VS("detail_wave");	break;
				case 1:	C.PassSET_VS("detail_still");	break;
				}
				
				// Stage1 - Base texture
				C.StageBegin		();
				C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_DIFFUSE);
				C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				C.StageSET_TMC		(oT_Name,"$null","$null",0);
				C.StageEnd			();
			}
			C.PassEnd			();
		} else {
			C.PassBegin		();
			{
				C.PassSET_ZB		(TRUE,TRUE);
				if (oBlend.value)	C.PassSET_Blend_BLEND	(TRUE, 200);
				else				C.PassSET_Blend_SET		(TRUE, 200);
				C.PassSET_LightFog	(FALSE,FALSE);

				switch (C.iLOD)
				{
				case 0:	C.PassSET_VS("detail_wave");	break;
				case 1:	C.PassSET_VS("detail_still");	break;
				}
				
				// Stage1 - Base texture
				C.StageBegin		();
				C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_DIFFUSE);
				C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_DIFFUSE);
				C.StageSET_TMC		(oT_Name,"$null","$null",0);
				C.StageEnd			();
			}
			C.PassEnd			();
		}
	}
}
