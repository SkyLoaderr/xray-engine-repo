// BlenderDefault.cpp: implementation of the CBlender_LaEmB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "blender_LaEmB.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_LaEmB::CBlender_LaEmB	()
{
	description.CLS		= B_LaEmB;
	strcpy				(oT2_Name,	"$null");
	strcpy				(oT2_xform,	"$null");
	strcpy				(oT2_const,	"$null");
}

CBlender_LaEmB::~CBlender_LaEmB	()
{
	
}

void	CBlender_LaEmB::Save(	CFS_Base& FS )
{
	CBlender::Save	(FS);
	xrPWRITE_MARKER	(FS,"Environment map");
	xrPWRITE_PROP	(FS,"Name",				xrPID_TEXTURE,	oT2_Name);
	xrPWRITE_PROP	(FS,"Transform",		xrPID_MATRIX,	oT2_xform);
	xrPWRITE_PROP	(FS,"Constant",			xrPID_CONSTANT,	oT2_const);
}

void	CBlender_LaEmB::Load(	CStream& FS, WORD version )
{
	CBlender::Load	(FS,version);
	xrPREAD_MARKER	(FS);
	xrPREAD_PROP	(FS,xrPID_TEXTURE,	oT2_Name);
	xrPREAD_PROP	(FS,xrPID_MATRIX,	oT2_xform);
	xrPREAD_PROP	(FS,xrPID_CONSTANT,	oT2_const);
}

void	CBlender_LaEmB::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	if (C.bEditor)	{
		if (0==stricmp(oT2_const,"$null"))
		{
			// NO CONSTANT
			
			C.PassBegin		();
			{
				C.PassSET_ZB			(TRUE,TRUE);
				C.PassSET_Blend_SET	();
				C.PassSET_LightFog	(TRUE,TRUE);
				
				// Stage1 - Env texture
				C.StageBegin		();
				{
					C.StageSET_Address	(D3DTADDRESS_WRAP);
					C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_DIFFUSE);
					C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_DIFFUSE);
					C.Stage_Texture		(oT2_Name,		C.L_textures		);
					C.Stage_Matrix		(oT2_xform,		C.L_matrices,		0);
					C.Stage_Constant		("$null",		C.L_constants		);
				}
				C.StageEnd			();
				
				// Stage2 - Base texture
				C.StageBegin		();
				{
					C.StageSET_Address	(D3DTADDRESS_WRAP);
					C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
					C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
					C.Stage_Texture		(oT_Name,		C.L_textures		);
					C.Stage_Matrix		(oT_xform,		C.L_matrices,		0);
					C.Stage_Constant		("$null",		C.L_constants		);
				}
				C.StageEnd			();
			}
			C.PassEnd			();
		} else {
			// WITH CONSTANT
			
			// Pass0 - (lmap+env*const)
			C.PassBegin		();
			{
				C.PassSET_ZB			(TRUE,TRUE);
				C.PassSET_Blend_SET	();
				C.PassSET_LightFog	(TRUE,TRUE);
				
				// Stage1 - Env texture * constant
				C.StageBegin		();
				{
					C.StageSET_Address	(D3DTADDRESS_WRAP);
					C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
					C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
					C.Stage_Texture		(oT2_Name,		C.L_textures		);
					C.Stage_Matrix		(oT2_xform,		C.L_matrices,		0);
					C.Stage_Constant		(oT2_const,		C.L_constants		);
				}
				C.StageEnd			();
				
				// Stage2 - Diffuse color
				C.StageBegin		();
				{
					C.StageSET_Address	(D3DTADDRESS_WRAP);
					C.StageSET_Color		(D3DTA_DIFFUSE,	  D3DTOP_ADD,			D3DTA_CURRENT);
					C.StageSET_Alpha		(D3DTA_DIFFUSE,	  D3DTOP_ADD,			D3DTA_CURRENT);
					C.Stage_Texture		("$null",		C.L_textures		);
					C.Stage_Matrix		("$null",		C.L_matrices,		0);
					C.Stage_Constant		("$null",		C.L_constants		);
				}
				C.StageEnd			();
			}
			C.PassEnd			();
			
			// Pass1 - *base
			C.PassBegin		();
			{
				C.PassSET_ZB			(TRUE,FALSE);
				C.PassSET_Blend_MUL	();
				C.PassSET_LightFog	(FALSE,TRUE);
				
				// Stage2 - Diffuse color
				C.StageBegin		();
				{
					C.StageSET_Address	(D3DTADDRESS_WRAP);
					C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.Stage_Texture		(oT_Name,		C.L_textures		);
					C.Stage_Matrix		(oT_xform,		C.L_matrices,		0);
					C.Stage_Constant		("$null",		C.L_constants		);
				}
				C.StageEnd			();
			}
			C.PassEnd			();
		}
	} else {
		switch (HW.Caps.pixel.dwStages)
		{
		case 2:		// Geforce1/2/MX
			if (0==stricmp(oT2_const,"$null"))	compile_2	(C);
			else								compile_2c	(C);
			break;
		case 3:		// Kyro, Radeon, Radeon2, Geforce3/4
		default:
			if (0==stricmp(oT2_const,"$null"))	compile_3	(C);
			else								compile_3c	(C);
			break;
		}
	}
}

//
void CBlender_LaEmB::compile_2	(CBlender_Compile& C)
{
	C.PassBegin		();
	{
		C.PassSET_ZB		(TRUE,TRUE);
		C.PassSET_Blend_SET();
		C.PassSET_LightFog	(FALSE,TRUE);
		
		// Stage0 - Lightmap
		C.StageBegin		();
		{
			C.StageSET_Address	(D3DTADDRESS_WRAP);
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.Stage_Texture		("$base1",		C.L_textures	);
			C.Stage_Matrix		("$null",		C.L_matrices,	1);
			C.Stage_Constant		("$null",		C.L_constants	);
		}
		C.StageEnd			();
		
		// Stage1 - Environment map
		C.StageBegin		();
		{
			C.StageSET_Address	(D3DTADDRESS_WRAP);
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			C.Stage_Texture		(oT2_Name,		C.L_textures	);
			C.Stage_Matrix		(oT2_xform,		C.L_matrices,	0);
			C.Stage_Constant		("$null",		C.L_constants	);
		}
		C.StageEnd			();
	}
	C.PassEnd			();
	
	// Pass2 - Base map
	C.PassBegin		();
	{
		C.PassSET_ZB			(TRUE,FALSE);
		C.PassSET_Blend_MUL	();
		C.PassSET_LightFog	(FALSE,TRUE);
		
		// Stage0 - Detail
		C.StageBegin		();
		{
			C.StageSET_Address	(D3DTADDRESS_WRAP);
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.Stage_Texture		(oT_Name,		C.L_textures	);
			C.Stage_Matrix		(oT_xform,		C.L_matrices,	0);
			C.Stage_Constant		("$null",		C.L_constants	);
		}
		C.StageEnd			();
	}
	C.PassEnd			();
}
//
void CBlender_LaEmB::compile_2c	(CBlender_Compile& C)
{
	C.PassBegin		();
	{
		C.PassSET_ZB		(TRUE,TRUE);
		C.PassSET_Blend_SET();
		C.PassSET_LightFog	(FALSE,TRUE);
		
		// Stage0 - Environment map [*] const
		C.StageBegin		();
		{
			C.StageSET_Address	(D3DTADDRESS_WRAP);
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			C.Stage_Texture		(oT2_Name,		C.L_textures	);
			C.Stage_Matrix		(oT2_xform,		C.L_matrices,	0);
			C.Stage_Constant		(oT2_const,		C.L_constants	);
		}
		C.StageEnd			();

		// Stage1 - [+] Lightmap
		C.StageBegin		();
		{
			C.StageSET_Address	(D3DTADDRESS_WRAP);
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			C.Stage_Texture		("$base1",		C.L_textures	);
			C.Stage_Matrix		("$null",		C.L_matrices,	1);
			C.Stage_Constant		("$null",		C.L_constants	);
		}
		C.StageEnd			();
	}
	C.PassEnd			();
	
	// Pass2 - Base map
	C.PassBegin		();
	{
		C.PassSET_ZB		(TRUE,FALSE);
		C.PassSET_Blend_MUL();
		C.PassSET_LightFog	(FALSE,TRUE);
		
		// Stage0 - Detail
		C.StageBegin		();
		{
			C.StageSET_Address	(D3DTADDRESS_WRAP);
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.Stage_Texture		(oT_Name,		C.L_textures	);
			C.Stage_Matrix		(oT_xform,		C.L_matrices,	0);
			C.Stage_Constant		("$null",		C.L_constants	);
		}
		C.StageEnd			();
	}
	C.PassEnd			();
}
//
void CBlender_LaEmB::compile_3	(CBlender_Compile& C)
{
	C.PassBegin		();
	{
		C.PassSET_ZB		(TRUE,TRUE);
		C.PassSET_Blend_SET();
		C.PassSET_LightFog	(FALSE,TRUE);
		
		// Stage0 - [=] Lightmap
		C.StageBegin		();
		{
			C.StageSET_Address	(D3DTADDRESS_WRAP);
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.Stage_Texture		("$base1",		C.L_textures	);
			C.Stage_Matrix		("$null",		C.L_matrices, 1);
			C.Stage_Constant		("$null",		C.L_constants	);
		}
		C.StageEnd			();
		
		// Stage1 - [+] Env-map
		C.StageBegin		();
		{
			C.StageSET_Address	(D3DTADDRESS_WRAP);
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			C.Stage_Texture		(oT2_Name,		C.L_textures	);
			C.Stage_Matrix		(oT2_xform,		C.L_matrices,	0);
			C.Stage_Constant		("$null",		C.L_constants	);
		}
		C.StageEnd			();
		
		// Stage2 - [*] Base
		C.StageBegin		();
		{
			C.StageSET_Address	(D3DTADDRESS_WRAP);
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
			C.Stage_Texture		(oT_Name,		C.L_textures	);
			C.Stage_Matrix		(oT_xform,		C.L_matrices,	0);
			C.Stage_Constant		("$null",		C.L_constants	);
		}
		C.StageEnd			();
	}
	C.PassEnd			();
}
//
void CBlender_LaEmB::compile_3c	(CBlender_Compile& C)
{
	C.PassBegin		();
	{
		C.PassSET_ZB		(TRUE,TRUE);
		C.PassSET_Blend_SET();
		C.PassSET_LightFog	(FALSE,TRUE);
		
		// Stage1 - [=] Env-map [*] const
		C.StageBegin		();
		{
			C.StageSET_Address	(D3DTADDRESS_WRAP);
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			C.Stage_Texture		(oT2_Name,		C.L_textures	);
			C.Stage_Matrix		(oT2_xform,		C.L_matrices,	0);
			C.Stage_Constant		(oT2_const,		C.L_constants	);
		}
		C.StageEnd			();
		
		// Stage0 - [=] Lightmap
		C.StageBegin		();
		{
			C.StageSET_Address	(D3DTADDRESS_WRAP);
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			C.Stage_Texture		("$base1",		C.L_textures	);
			C.Stage_Matrix		("$null",		C.L_matrices,	1);
			C.Stage_Constant		("$null",		C.L_constants	);
		}
		C.StageEnd			();
		
		// Stage2 - [*] Base
		C.StageBegin		();
		{
			C.StageSET_Address	(D3DTADDRESS_WRAP);
			C.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
			C.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
			C.Stage_Texture		(oT_Name,		C.L_textures	);
			C.Stage_Matrix		(oT_xform,		C.L_matrices,	0);
			C.Stage_Constant		("$null",		C.L_constants	);
		}
		C.StageEnd			();
	}
	C.PassEnd			();
}