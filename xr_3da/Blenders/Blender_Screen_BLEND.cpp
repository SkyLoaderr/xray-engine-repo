#include "stdafx.h"
#pragma hdrstop

#include "Blender_Screen_BLEND.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Screen_BLEND::CBlender_Screen_BLEND()
{
	description.CLS		= B_SCREEN_BLEND;
	oAREF.value			= 32;
	oAREF.min			= 0;
	oAREF.max			= 255;
}

CBlender_Screen_BLEND::~CBlender_Screen_BLEND()
{
	
}

void	CBlender_Screen_BLEND::Save	( CFS_Base& FS	)
{
	CBlender::Save	(FS);
	BP_WRITE		("Alpha ref",	BPID_INTEGER,	oAREF);
}

void	CBlender_Screen_BLEND::Load	( CStream& FS	)
{
	CBlender::Load	(FS);
	BP_READ			(BPID_INTEGER,		oAREF);
}

void	CBlender_Screen_BLEND::Compile	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices)
{
	RS.PassBegin		();
	{
		RS.PassSET_ZB		(FALSE,FALSE);
		RS.PassSET_Blend	(TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,TRUE,oAREF.value);
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
