// BlenderDefault.cpp: implementation of the CBlender_default class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "BlenderDefault.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_default::CBlender_default	()
{
	description.CLS		= B_DEFAULT;
}

CBlender_default::~CBlender_default	()
{

}
void	CBlender_default::Save(	CFS_Base& FS )
{
	CBlender::Save	(FS);
}
void	CBlender_default::Load(	CStream& FS, WORD version )
{
	CBlender::Load	(FS,version);
}
void	CBlender_default::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	if (C.bEditor)	{
		C.PassBegin		();
		{
			C.PassSET_ZB			(TRUE,TRUE);
			C.PassSET_Blend			(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,FALSE,0);
			C.PassSET_LightFog		(TRUE,TRUE);
			
			// Stage1 - Base texture
			C.StageBegin			();
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			C.StageSET_TMC			(oT_Name,oT_xform,"$null",0);
			C.StageEnd				();
		}
		C.PassEnd			();
	} else {
		if (C.bLighting)	
		{
			// Lighting only
			C.PassBegin			();
			{
				C.PassSET_ZB			(TRUE,TRUE);
				C.PassSET_Blend_SET		();
				C.PassSET_LightFog		(FALSE,FALSE);
				
				// Stage0 - Lightmap
				C.StageBegin			();
				C.StageTemplate_LMAP0	();
				C.StageEnd				();
			}
			C.PassEnd			();
		} else {
			// Level view
			C.PassBegin		();
			{
				C.PassSET_ZB			(TRUE,TRUE);
				C.PassSET_Blend_SET		();
				C.PassSET_LightFog		(FALSE,TRUE);
				
				// Stage0 - Lightmap
				C.StageBegin			();
				C.StageTemplate_LMAP0	();
				C.StageEnd				();
				
				// Stage1 - Base texture
				C.StageBegin			();
				C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
				C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
				C.StageSET_TMC			(oT_Name,oT_xform,"$null",0);
				C.StageEnd				();
			}
			C.PassEnd			();
		}
	}
}
