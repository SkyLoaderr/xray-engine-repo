#include "stdafx.h"
#pragma hdrstop

#include "Blender_Shadow_Texture.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_ShTex::CBlender_ShTex()
{
	description.CLS		= B_SHADOW_TEX;
}

CBlender_ShTex::~CBlender_ShTex()
{

}

void	CBlender_ShTex::Save	( CFS_Base& FS	)
{
	CBlender::Save	(FS);
}

void	CBlender_ShTex::Load	( CStream& FS, WORD version	)
{
	CBlender::Load	(FS,version);
}

void CBlender_ShTex::Compile	(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	C.PassBegin		();
	{
		C.PassSET_ZB		(FALSE, FALSE);
		C.PassSET_Blend_SET	();
		C.PassSET_LightFog	(FALSE, FALSE);

		// Stage0 - Base texture
		C.StageBegin		();
		C.StageSET_Color	(D3DTA_TFACTOR,	  D3DTOP_SELECTARG1,	D3DTA_TFACTOR);
		C.StageSET_Alpha	(D3DTA_TFACTOR,	  D3DTOP_SELECTARG1,	D3DTA_TFACTOR);
		C.Stage_Texture		("$null");
		C.Stage_Matrix		("$null",0);
		C.Stage_Constant	("$null");
		C.StageEnd			();

		// 
		C.R().SetRS			(D3DRS_TEXTUREFACTOR,0);
	}
	C.PassEnd			();
}
