#include "stdafx.h"
#pragma hdrstop

#include "Blender_Screen_SET.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Screen_SET::CBlender_Screen_SET()
{
	description.CLS		= B_SCREEN_SET;
	oBlend.Count		= 3;
	oBlend.IDselected	= 0;
	oAREF.value			= 32;
	oAREF.min			= 0;
	oAREF.max			= 255;
}

CBlender_Screen_SET::~CBlender_Screen_SET()
{
	
}

void	CBlender_Screen_SET::Save	( CFS_Base& FS	)
{
	CBlender::Save	(FS);
	BP_WRITE		("Alpha ref",	BPID_INTEGER,	oAREF);
}

void	CBlender_Screen_SET::Load	( CStream& FS	)
{
	CBlender::Load	(FS);
	BP_READ			(BPID_INTEGER,		oAREF);
}

void	CBlender_Screen_SET::Compile	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param, BOOL bEditor)
{
	RS.PassBegin		();
	{
		RS.PassSET_ZB		(FALSE,FALSE);
		switch (oBlend.IDselected)
		{
		case 0:	// SET
			RS.PassSET_Blend	(FALSE,	D3DBLEND_ONE,D3DBLEND_ZERO,				FALSE,0);
			RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
			RS.R().SetRS		(D3DRS_FOGENABLE,					BC(FALSE));
			RS.StageBegin		();
			{
				RS.StageSET_Address	(D3DTADDRESS_CLAMP);
				RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				RS.Stage_Texture	(oT_Name,	L_textures);
				RS.Stage_Matrix		("$null",	L_matrices,	0);
				RS.Stage_Constant	("$null",	L_constants);
			}
			RS.StageEnd			();
			break;
		case 1: // BLEND
			RS.PassSET_Blend	(TRUE,	D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,	TRUE,oAREF.value);
			RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
			RS.R().SetRS		(D3DRS_FOGENABLE,					BC(FALSE));
			RS.StageBegin		();
			{
				RS.StageSET_Address	(D3DTADDRESS_CLAMP);
				RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				RS.Stage_Texture	(oT_Name,	L_textures);
				RS.Stage_Matrix		("$null",	L_matrices,	0);
				RS.Stage_Constant	("$null",	L_constants);
			}
			RS.StageEnd			();
			break;
		case 2:	// ADD
			RS.PassSET_Blend	(TRUE,	D3DBLEND_ONE,D3DBLEND_ONE,				FALSE,oAREF.value);
			RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
			RS.R().SetRS		(D3DRS_FOGENABLE,					BC(FALSE));
			RS.StageBegin		();
			{
				RS.StageSET_Address	(D3DTADDRESS_CLAMP);
				RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				RS.Stage_Texture	(oT_Name,	L_textures);
				RS.Stage_Matrix		("$null",	L_matrices,	0);
				RS.Stage_Constant	("$null",	L_constants);
			}
			RS.StageEnd			();
			break;
		}
	}
	RS.PassEnd			();
}
