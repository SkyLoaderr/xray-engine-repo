#include "stdafx.h"
#pragma hdrstop

#include "Blender_Blur.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Blur::CBlender_Blur()
{
	description.CLS		= B_SHADOW_WORLD;
}

CBlender_Blur::~CBlender_Blur()
{

}

void	CBlender_Blur::Save	( CFS_Base& FS	)
{
	CBlender::Save	(FS);
}

void	CBlender_Blur::Load	( CStream& FS, WORD version	)
{
	CBlender::Load	(FS,version);
}

void CBlender_Blur::Compile	(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	C.PassBegin		();
	{
		C.PassSET_ZB		(TRUE, FALSE);
		C.PassSET_Blend_MUL	();
		C.PassSET_LightFog	(FALSE, FALSE);

		// Stage0 - Base texture
		C.StageBegin		();
		C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_ADD,	D3DTA_DIFFUSE);
		C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_ADD,	D3DTA_DIFFUSE);
		C.Stage_Texture		("$base0");
		C.Stage_Matrix		("$null",0);
		C.Stage_Constant	("$null");
		C.StageEnd			();
	}
	C.PassEnd			();
}
