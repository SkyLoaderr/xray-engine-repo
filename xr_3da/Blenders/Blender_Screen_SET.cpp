#include "stdafx.h"
#pragma hdrstop

#include "Blender_Screen_SET.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Screen_SET::CBlender_Screen_SET()
{
	description.CLS		= B_SCREEN_SET;
}

CBlender_Screen_SET::~CBlender_Screen_SET()
{
	
}

void	CBlender_Screen_SET::Save	( CFS_Base& FS	)
{
	CBlender::Save	(FS);
}

void	CBlender_Screen_SET::Load	( CStream& FS	)
{
	CBlender::Load	(FS);
}

void	CBlender_Screen_SET::Compile	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices)
{
	RS.PassBegin		();
	{
		RS.PassSET_ZB		(FALSE,FALSE);
		RS.PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
		RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
		RS.R().SetRS		(D3DRS_FOGENABLE,					BC(FALSE));
		
		// Stage0 - Base texture
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_CLAMP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			RS.Stage_Texture	(oT_Name,	L_textures);
			RS.Stage_Matrix		(oT_xform,	L_matrices,	0);
			RS.Stage_Constant	("$null",	L_constants);
		}
		RS.StageEnd			();
	}
	RS.PassEnd			();
}
