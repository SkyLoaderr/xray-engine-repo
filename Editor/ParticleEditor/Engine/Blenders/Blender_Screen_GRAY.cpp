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

void	CBlender_Screen_GRAY::Load	( CStream& FS, WORD version	)
{
	CBlender::Load	(FS,version);
}

void	CBlender_Screen_GRAY::Compile	(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	C.RS.PassBegin		();
	{
		C.RS.PassSET_ZB			(FALSE,FALSE);
		C.RS.PassSET_Blend		(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
		C.RS.PassSET_LightFog	(FALSE,FALSE);
		
		if (HW.Caps.pixel.op_DP3 && !strstr(GetCommandLine(),"-no_dp3") && !strstr(GetCommandLine(),"-NO_DP3"))	
		{
			C.RS.R().SetRS		(D3DRS_TEXTUREFACTOR,D3DCOLOR_RGBA(76+105,150+105,29+105,0));

			// Stage0 - Base texture
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_CLAMP);
				C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_DIFFUSE);
				C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_DIFFUSE);
				C.RS.Stage_Texture		(oT_Name,	C.L_textures);
				C.RS.Stage_Matrix		(oT_xform,	C.L_matrices,	0);
				C.RS.Stage_Constant		("$null",	C.L_constants);
			}
			C.RS.StageEnd			();
			
			// Stage1 - Base texture
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_CLAMP);
				C.RS.StageSET_Color		(D3DTA_CURRENT,	  D3DTOP_DOTPRODUCT3,	D3DTA_TFACTOR);
				C.RS.StageSET_Alpha		(D3DTA_CURRENT,	  D3DTOP_DOTPRODUCT3,	D3DTA_TFACTOR);
				C.RS.Stage_Texture		(oT_Name,	C.L_textures);
				C.RS.Stage_Matrix		(oT_xform,	C.L_matrices,	0);
				C.RS.Stage_Constant		("$null",	C.L_constants);
			}
			C.RS.StageEnd			();
		} else {
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_CLAMP);
				C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				C.RS.Stage_Texture		(oT_Name,	C.L_textures);
				C.RS.Stage_Matrix		(oT_xform,	C.L_matrices,	0);
				C.RS.Stage_Constant		("$null",	C.L_constants);
			}
			C.RS.StageEnd			();
		}
	}
	C.RS.PassEnd			();
}
