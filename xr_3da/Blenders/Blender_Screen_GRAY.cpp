#include "stdafx.h"
#pragma hdrstop

#include "Blender_Screen_GRAY.h"

// Y =  0.299*Red+0.587*Green+0.114*Blue
// Y =  76.544*R +150.272*G  +29.184*B
// Y =  76(4C)    150(96)     29(1D)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Screen_GRAY::CBlender_Screen_GRAY()
{
	description.CLS		= B_SCREEN_GRAY;
}

CBlender_Screen_GRAY::~CBlender_Screen_GRAY()
{
	
}

void	CBlender_Screen_GRAY::Save	( CFS_Base& FS	)
{
	CBlender::Save	(FS);
}

void	CBlender_Screen_GRAY::Load	( CStream& FS	)
{
	CBlender::Load	(FS);
}

void	CBlender_Screen_GRAY::Compile	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices)
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
			if (HW.Caps.pixel.op_DP3)	{
				RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_DOTPRODUCT3,	D3DTA_TFACTOR);
				RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_DOTPRODUCT3,	D3DTA_TFACTOR);
				RS.R().SetRS		(D3DRS_TEXTUREFACTOR,D3DCOLOR_RGBA(76,150,29,0));
			} else {
				// 
				RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			}
			RS.Stage_Texture	(oT_Name,	L_textures);
			RS.Stage_Matrix		(oT_xform,	L_matrices,	0);
			RS.Stage_Constant	("$null",	L_constants);
		}
		RS.StageEnd			();
	}
	RS.PassEnd			();
}
