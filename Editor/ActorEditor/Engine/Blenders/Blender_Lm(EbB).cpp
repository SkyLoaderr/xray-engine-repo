// BlenderDefault.cpp: implementation of the CBlender_LmEbB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "blender_Lm(EbB).h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_LmEbB::CBlender_LmEbB	()
{
	description.CLS		= B_LmEbB;
	strcpy				(oT2_Name,	"$null");
	strcpy				(oT2_xform,	"$null");
}

CBlender_LmEbB::~CBlender_LmEbB	()
{
	
}

void	CBlender_LmEbB::Save(	CFS_Base& FS )
{
	CBlender::Save	(FS);
	xrPWRITE_MARKER	(FS,"Environment map");
	xrPWRITE_PROP	(FS,"Name",				xrPID_TEXTURE,	oT2_Name);
	xrPWRITE_PROP	(FS,"Transform",		xrPID_MATRIX,	oT2_xform);
}

void	CBlender_LmEbB::Load(	CStream& FS )
{
	CBlender::Load	(FS);
	xrPREAD_MARKER	(FS);
	xrPREAD_PROP	(FS,xrPID_TEXTURE,	oT2_Name);
	xrPREAD_PROP	(FS,xrPID_MATRIX,	oT2_xform);
}

void	CBlender_LmEbB::Compile(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param, BOOL bEditor)
{
	if (bEditor)	{
		// NO CONSTANT
		
		RS.PassBegin		();
		{
			RS.PassSET_ZB		(TRUE,TRUE);
			RS.PassSET_Blend_SET();
			RS.R().SetRS		(D3DRS_LIGHTING,					BC(TRUE));
			RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
			
			// Stage1 - Env texture
			RS.StageBegin		();
			{
				RS.StageSET_Address	(D3DTADDRESS_WRAP);
				RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
				RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
				RS.Stage_Texture	(oT2_Name,		L_textures		);
				RS.Stage_Matrix		(oT2_xform,		L_matrices,		0);
				RS.Stage_Constant	("$null",		L_constants		);
			}
			RS.StageEnd			();
			
			// Stage2 - Base texture
			RS.StageBegin		();
			{
				RS.StageSET_Address	(D3DTADDRESS_WRAP);
				RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_BLENDTEXTUREALPHA,	D3DTA_CURRENT);
				RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_CURRENT);
				RS.Stage_Texture	(oT_Name,		L_textures		);
				RS.Stage_Matrix		(oT_xform,		L_matrices,		0);
				RS.Stage_Constant	("$null",		L_constants		);
			}
			RS.StageEnd			();

			// Stage3 - Lighting - should work on all 2tex hardware
			RS.StageBegin		();
			{
				RS.StageSET_Address	(D3DTADDRESS_WRAP);
				RS.StageSET_Color	(D3DTA_DIFFUSE,	  D3DTOP_MODULATE,			D3DTA_CURRENT);
				RS.StageSET_Alpha	(D3DTA_DIFFUSE,	  D3DTOP_SELECTARG2,		D3DTA_CURRENT);
				RS.Stage_Texture	("$null",		L_textures		);
				RS.Stage_Matrix		("$null",		L_matrices,		0);
				RS.Stage_Constant	("$null",		L_constants		);
			}
			RS.StageEnd			();
		}
		RS.PassEnd			();
	} else {
		switch (HW.Caps.pixel.dwStages)
		{
		case 2:		// Geforce1/2/MX
			compile_2	(RS,L_textures,L_matrices,L_constants);
			break;
		case 3:		// Kyro, Radeon, Radeon2, Geforce3/4
		default:
			compile_3	(RS,L_textures,L_matrices,L_constants);
			break;
		}
	}
}

//
void CBlender_LmEbB::compile_2	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param)
{
	// Pass1 - Lighting
	RS.PassBegin		();
	{
		RS.PassSET_ZB		(TRUE,TRUE);
		RS.PassSET_Blend_SET();
		RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
		RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
		
		// Stage0 - Detail
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
			RS.Stage_Texture	("$base1",		L_textures	);
			RS.Stage_Matrix		("$null",		L_matrices,	1);
			RS.Stage_Constant	("$null",		L_constants	);
		}
		RS.StageEnd			();
	}
	RS.PassEnd			();

	// Pass2 - (env^base)
	RS.PassBegin		();
	{
		RS.PassSET_ZB		(TRUE,FALSE);
		RS.PassSET_Blend_MUL();
		RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
		RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
		
		// Stage0 - Environment map
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
			RS.Stage_Texture	(oT2_Name,		L_textures	);
			RS.Stage_Matrix		(oT2_xform,		L_matrices,	0);
			RS.Stage_Constant	("$null",		L_constants	);
		}
		RS.StageEnd			();

		// Stage1 - Base map
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_BLENDTEXTUREALPHA,	D3DTA_CURRENT);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_CURRENT);
			RS.Stage_Texture	(oT_Name,		L_textures	);
			RS.Stage_Matrix		(oT_xform,		L_matrices,	0);
			RS.Stage_Constant	("$null",		L_constants	);
		}
		RS.StageEnd			();
	}
	RS.PassEnd			();
}

//
void CBlender_LmEbB::compile_3	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param)
{
	RS.PassBegin		();
	{
		RS.PassSET_ZB		(TRUE,TRUE);
		RS.PassSET_Blend_SET();
		RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
		RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
		
		// Stage0 - Environment map
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
			RS.Stage_Texture	(oT2_Name,		L_textures	);
			RS.Stage_Matrix		(oT2_xform,		L_matrices,	0);
			RS.Stage_Constant	("$null",		L_constants	);
		}
		RS.StageEnd			();
		
		// Stage1 - [^] Base map
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_BLENDTEXTUREALPHA,	D3DTA_CURRENT);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_CURRENT);
			RS.Stage_Texture	(oT_Name,		L_textures	);
			RS.Stage_Matrix		(oT_xform,		L_matrices,	0);
			RS.Stage_Constant	("$null",		L_constants	);
		}
		RS.StageEnd			();
		
		// Stage2 - [*] Lightmap
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,			D3DTA_CURRENT);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,		D3DTA_CURRENT);
			RS.Stage_Texture	("$base1",		L_textures	);
			RS.Stage_Matrix		("$null",		L_matrices, 1);
			RS.Stage_Constant	("$null",		L_constants	);
		}
		RS.StageEnd			();
	}
	RS.PassEnd			();
}
