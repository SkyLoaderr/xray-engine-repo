#include "stdafx.h"
#pragma hdrstop

#include "Blender_Shadow_World.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_ShWorld::CBlender_ShWorld()
{
	description.CLS		= B_SHADOW_WORLD;
}

CBlender_ShWorld::~CBlender_ShWorld()
{

}

void	CBlender_ShWorld::Save	( IWriter& FS	)
{
	CBlender::Save	(FS);
}

void	CBlender_ShWorld::Load	( IReader& FS, WORD version	)
{
	CBlender::Load	(FS,version);
}

void CBlender_ShWorld::Compile	(CBlender_Compile& C)
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
