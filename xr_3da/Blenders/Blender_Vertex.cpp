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

void	CBlender_Vertex::Save	( CFS_Base& FS	)
{
	CBlender::Save	(FS);
}

void	CBlender_Vertex::Load	( CStream& FS, WORD version	)
{
	CBlender::Load	(FS,version);
}

void CBlender_Vertex::Compile	(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	if (C.bLighting)	
	{
		// Lighting only
		C.RS.PassBegin		();
		{
			C.RS.PassSET_ZB			(TRUE,TRUE);
			C.RS.PassSET_Blend_SET	();
			C.RS.PassSET_LightFog	(FALSE,FALSE);
			
			// Stage0 - diffuse
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
				C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_DIFFUSE);
				C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_DIFFUSE);
				C.RS.Stage_Texture		("$null",	C.L_textures	);
				C.RS.Stage_Matrix		(oT_xform,	C.L_matrices,	0);
				C.RS.Stage_Constant		("$null",	C.L_constants	);
			}
			C.RS.StageEnd			();
		}
		C.RS.PassEnd			();
	} else {
		// Level shader
		C.RS.PassBegin		();
		{
			C.RS.PassSET_ZB			(TRUE,TRUE);
			C.RS.PassSET_Blend		(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
			C.RS.PassSET_LightFog	(C.bEditor,TRUE);
			
			// Stage0 - Base texture
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
				C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_DIFFUSE);
				C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_DIFFUSE);
				C.RS.Stage_Texture		(oT_Name,	C.L_textures	);
				C.RS.Stage_Matrix		(oT_xform,	C.L_matrices,	0);
				C.RS.Stage_Constant		("$null",	C.L_constants	);
			}
			C.RS.StageEnd			();
		}
		C.RS.PassEnd			();
	}
}
