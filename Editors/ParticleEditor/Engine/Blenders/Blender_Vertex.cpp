// Blender_Vertex.cpp: implementation of the CBlender_Vertex class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Blender_Vertex.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Vertex::CBlender_Vertex()
{
	description.CLS		= B_VERT;
}

CBlender_Vertex::~CBlender_Vertex()
{
	
}

void	CBlender_Vertex::Save	( IWriter& FS	)
{
	CBlender::Save	(FS);
}

void	CBlender_Vertex::Load	( IReader& FS, WORD version	)
{
	CBlender::Load	(FS,version);
}

void CBlender_Vertex::Compile	(CBlender_Compile& C)
{
	CBlender::Compile		(C);

	if (C.bEditor)
	{
		// Editor shader
		C.PassBegin		();
		{
			C.PassSET_ZB			(TRUE,TRUE);
			C.PassSET_Blend			(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
			C.PassSET_LightFog		(TRUE,TRUE);
			
			// Stage0 - Base texture
			C.StageBegin			();
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,	D3DTA_DIFFUSE);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,	D3DTA_DIFFUSE);
			C.Stage_Texture			(oT_Name);
			C.Stage_Matrix			(oT_xform,0);
			C.Stage_Constant		("$null");
			C.StageEnd				();
		}
		C.PassEnd			();
	} else {
		if (C.bLighting)	
		{
			// Lighting only
			C.PassBegin		();
			{
				C.PassSET_ZB			(TRUE,TRUE);
				C.PassSET_Blend_SET		();
				C.PassSET_LightFog		(FALSE,FALSE);
				
				// Stage0 - diffuse
				C.StageBegin			();
				C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_DIFFUSE);
				C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_DIFFUSE);
				C.Stage_Texture			("$null");
				C.Stage_Matrix			("$null",0);
				C.Stage_Constant		("$null");
				C.StageEnd				();
			}
			C.PassEnd			();
		} else {
			// Level shader
			C.PassBegin		();
			{
				C.PassSET_ZB			(TRUE,TRUE);
				C.PassSET_Blend			(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
				C.PassSET_LightFog		(C.bEditor,TRUE);
				
				// Stage0 - Base texture
				C.StageBegin			();
				C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_DIFFUSE);
				C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_DIFFUSE);
				C.Stage_Texture			(oT_Name);
				C.Stage_Matrix			(oT_xform,0);
				C.Stage_Constant		("$null");
				C.StageEnd				();
			}
			C.PassEnd			();
		}
	}
}
