// BlenderDefault.cpp: implementation of the CBlender_complex2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "blender_complex2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_complex2::CBlender_complex2	()
{
	description.CLS		= B_COMPLEX2;
}

CBlender_complex2::~CBlender_complex2	()
{

}

void	CBlender_complex2::Save(	CFS_Base& FS )
{
	CBlender::Save	(FS);
}
void	CBlender_complex2::Load(	CStream& FS )
{
	CBlender::Load	(FS);
}
void	CBlender_complex2::Compile(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param, BOOL bEditor)
{
	if (bEditor)	{
		RS.PassBegin		();
		{
			RS.PassSET_ZB		(TRUE,TRUE);
			RS.PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
			RS.R().SetRS		(D3DRS_LIGHTING,					BC(TRUE));
			RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
			
			// Stage1 - Base texture
			RS.StageBegin		();
			{
				RS.StageSET_Address	(D3DTADDRESS_WRAP);
				RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				RS.Stage_Texture	(oT_Name,	L_textures		);
				RS.Stage_Matrix		(oT_xform,	L_matrices,		0);
				RS.Stage_Constant	("$null",	L_constants		);
			}
			RS.StageEnd			();

			// Stage2 - Second texture
			RS.StageBegin		();
			{
				RS.StageSET_Address	(D3DTADDRESS_WRAP);
				RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				RS.Stage_Texture	(oT2_Name,		L_textures	);
				RS.Stage_Matrix		(oT2_xform,		L_matrices,	0);
				RS.Stage_Constant	(oT2_Constant,	L_constants	);
			}
			RS.StageEnd			();
		}
		RS.PassEnd			();
	} else {
		switch (oBlend.IDselected)	{
		case 0:	// blend	(usually cubemap^base)
			break;
		case 1:	// add		(usually some type of "light" + base)
			break;
		case 2:	// mul2X	(usually detail)
			switch (HW.Caps.caps_Texturing.dwStages)
			{
			case 2:		// Geforce1/2/MX
				RS.PassBegin		();
				{
					RS.PassSET_ZB		(TRUE,TRUE);
					RS.PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
					RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
					RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
					
					// Stage0 - Lightmap
					RS.StageBegin		();
					{
						RS.StageSET_Address	(D3DTADDRESS_WRAP);
						RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
						RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
						RS.Stage_Texture	("$base1",L_textures	);
						RS.Stage_Matrix		("$null",L_matrices,	1);
						RS.Stage_Constant	("$null",L_constants	);
					}
					RS.StageEnd			();
					
					// Stage1 - Base texture
					RS.StageBegin		();
					{
						RS.StageSET_Address	(D3DTADDRESS_WRAP);
						RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
						RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
						RS.Stage_Texture	(oT_Name,	L_textures	);
						RS.Stage_Matrix		(oT_xform,	L_matrices,	0);
						RS.Stage_Constant	("$null",	L_constants	);
					}
					RS.StageEnd			();
				}
				RS.PassEnd			();

				// Pass2 - Detail
				RS.PassBegin		();
				{
					RS.PassSET_ZB		(TRUE,FALSE);
					RS.PassSET_Blend	(TRUE,D3DBLEND_DESTCOLOR,D3DBLEND_SRCCOLOR,	FALSE,0);
					RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
					RS.R().SetRS		(D3DRS_FOGENABLE,					BC(FALSE));

					// Stage0 - Detail
					RS.StageBegin		();
					{
						RS.StageSET_Address	(D3DTADDRESS_WRAP);
						RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
						RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
						RS.Stage_Texture	(oT2_Name,		L_textures);
						RS.Stage_Matrix		(oT2_xform,		L_matrices,0);
						RS.Stage_Constant	(oT2_Constant,	L_constants);
					}
					RS.StageEnd			();
				}
				RS.PassEnd			();
				break;
			case 3:		// Kyro, Radeon, Radeon2, Geforce3/4
			default:
				RS.PassBegin		();
				{
					RS.PassSET_ZB		(TRUE,TRUE);
					RS.PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,FALSE,0);
					RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
					RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
					
					// Stage0 - Lightmap
					RS.StageBegin		();
					{
						RS.StageSET_Address	(D3DTADDRESS_WRAP);
						RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
						RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
						RS.Stage_Texture	("$base1",		L_textures);
						RS.Stage_Matrix		("$null",		L_matrices, 1);
						RS.Stage_Constant	("$null",		L_constants);
					}
					RS.StageEnd			();
					
					// Stage1 - Base texture
					RS.StageBegin		();
					{
						RS.StageSET_Address	(D3DTADDRESS_WRAP);
						RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
						RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_CURRENT);
						RS.Stage_Texture	(oT_Name,		L_textures);
						RS.Stage_Matrix		(oT_xform,		L_matrices,	0);
						RS.Stage_Constant	("$null",		L_constants);
					}
					RS.StageEnd			();
					
					// Stage2 - Detail
					RS.StageBegin		();
					{
						RS.StageSET_Address	(D3DTADDRESS_WRAP);
						RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
						RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_CURRENT);
						RS.Stage_Texture	(oT2_Name,		L_textures);
						RS.Stage_Matrix		(oT2_xform,		L_matrices,	0);
						RS.Stage_Constant	(oT2_Constant,	L_constants);
					}
					RS.StageEnd			();
				}
				RS.PassEnd			();
				break;
			}
			break;
		}
	}
}
