#include "stdafx.h"
#pragma hdrstop

#include "Blender_fat_flat.h"

CBlender_fat_flat::CBlender_fat_flat	()	{	description.CLS		= B_DEFAULT;	}
CBlender_fat_flat::~CBlender_fat_flat	()	{	}

void	CBlender_fat_flat::Save	(	IWriter& FS )
{
	CBlender::Save	(FS);
}
void	CBlender_fat_flat::Load	(	IReader& FS, WORD version )
{
	CBlender::Load	(FS,version);
}
void	CBlender_fat_flat::Compile(CBlender_Compile& C)
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
