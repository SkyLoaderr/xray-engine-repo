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
			
			C.RS.PassBegin		();
			{
				C.RS.PassSET_ZB			(TRUE,TRUE);
				C.RS.PassSET_Blend_SET	();
				C.RS.PassSET_LightFog	(TRUE,TRUE);
				
				// Stage1 - Env texture
				C.RS.StageBegin		();
				{
					C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
					C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_DIFFUSE);
					C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_DIFFUSE);
					C.RS.Stage_Texture		(oT2_Name,		C.L_textures		);
					C.RS.Stage_Matrix		(oT2_xform,		C.L_matrices,		0);
					C.RS.Stage_Constant		("$null",		C.L_constants		);
				}
				C.RS.StageEnd			();
				
				// Stage2 - Base texture
				C.RS.StageBegin		();
				{
					C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
					C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
					C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
					C.RS.Stage_Texture		(oT_Name,		C.L_textures		);
					C.RS.Stage_Matrix		(oT_xform,		C.L_matrices,		0);
					C.RS.Stage_Constant		("$null",		C.L_constants		);
				}
				C.RS.StageEnd			();
			}
			C.RS.PassEnd			();
		} else {
			// WITH CONSTANT
			
			// Pass0 - (lmap+env*const)
			C.RS.PassBegin		();
			{
				C.RS.PassSET_ZB			(TRUE,TRUE);
				C.RS.PassSET_Blend_SET	();
				C.RS.PassSET_LightFog	(TRUE,TRUE);
				
				// Stage1 - Env texture * constant
				C.RS.StageBegin		();
				{
					C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
					C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
					C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
					C.RS.Stage_Texture		(oT2_Name,		C.L_textures		);
					C.RS.Stage_Matrix		(oT2_xform,		C.L_matrices,		0);
					C.RS.Stage_Constant		(oT2_const,		C.L_constants		);
				}
				C.RS.StageEnd			();
				
				// Stage2 - Diffuse color
				C.RS.StageBegin		();
				{
					C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
					C.RS.StageSET_Color		(D3DTA_DIFFUSE,	  D3DTOP_ADD,			D3DTA_CURRENT);
					C.RS.StageSET_Alpha		(D3DTA_DIFFUSE,	  D3DTOP_ADD,			D3DTA_CURRENT);
					C.RS.Stage_Texture		("$null",		C.L_textures		);
					C.RS.Stage_Matrix		("$null",		C.L_matrices,		0);
					C.RS.Stage_Constant		("$null",		C.L_constants		);
				}
				C.RS.StageEnd			();
			}
			C.RS.PassEnd			();
			
			// Pass1 - *base
			C.RS.PassBegin		();
			{
				C.RS.PassSET_ZB			(TRUE,FALSE);
				C.RS.PassSET_Blend_MUL	();
				C.RS.PassSET_LightFog	(FALSE,TRUE);
				
				// Stage2 - Diffuse color
				C.RS.StageBegin		();
				{
					C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
					C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					C.RS.Stage_Texture		(oT_Name,		C.L_textures		);
					C.RS.Stage_Matrix		(oT_xform,		C.L_matrices,		0);
					C.RS.Stage_Constant		("$null",		C.L_constants		);
				}
				C.RS.StageEnd			();
			}
			C.RS.PassEnd			();
		}
	} else {
		switch (HW.Caps.pixel.dwStages)
		{
		case 2:		// Geforce1/2/MX
			if (0==stricmp(oT2_const,"$null"))	compile_2	(RS,L_textures,L_matrices,L_constants);
			else								compile_2c	(RS,L_textures,L_matrices,L_constants);
			break;
		case 3:		// Kyro, Radeon, Radeon2, Geforce3/4
		default:
			if (0==stricmp(oT2_const,"$null"))	compile_3	(RS,L_textures,L_matrices,L_constants);
			else								compile_3c	(RS,L_textures,L_matrices,L_constants);
			break;
		}
	}
}

//
void CBlender_LaEmB::compile_2	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param)
{
	C.RS.PassBegin		();
	{
		C.RS.PassSET_ZB		(TRUE,TRUE);
		C.RS.PassSET_Blend_SET();
		C.RS.PassSET_LightFog	(FALSE,TRUE);
		
		// Stage0 - Lightmap
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.RS.Stage_Texture	("$base1",		L_textures	);
			C.RS.Stage_Matrix		("$null",		L_matrices,	1);
			C.RS.Stage_Constant	("$null",		L_constants	);
		}
		C.RS.StageEnd			();
		
		// Stage1 - Environment map
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			C.RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			C.RS.Stage_Texture	(oT2_Name,		L_textures	);
			C.RS.Stage_Matrix		(oT2_xform,		L_matrices,	0);
			C.RS.Stage_Constant	("$null",		L_constants	);
		}
		C.RS.StageEnd			();
	}
	C.RS.PassEnd			();
	
	// Pass2 - Base map
	C.RS.PassBegin		();
	{
		C.RS.PassSET_ZB		(TRUE,FALSE);
		C.RS.PassSET_Blend_MUL();
		C.RS.PassSET_LightFog	(FALSE,TRUE);
		
		// Stage0 - Detail
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.RS.Stage_Texture	(oT_Name,		L_textures	);
			C.RS.Stage_Matrix		(oT_xform,		L_matrices,	0);
			C.RS.Stage_Constant	("$null",		L_constants	);
		}
		C.RS.StageEnd			();
	}
	C.RS.PassEnd			();
}
//
void CBlender_LaEmB::compile_2c	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param)
{
	C.RS.PassBegin		();
	{
		C.RS.PassSET_ZB		(TRUE,TRUE);
		C.RS.PassSET_Blend_SET();
		C.RS.PassSET_LightFog	(FALSE,TRUE);
		
		// Stage0 - Environment map [*] const
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			C.RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			C.RS.Stage_Texture	(oT2_Name,		L_textures	);
			C.RS.Stage_Matrix		(oT2_xform,		L_matrices,	0);
			C.RS.Stage_Constant	(oT2_const,		L_constants	);
		}
		C.RS.StageEnd			();

		// Stage1 - [+] Lightmap
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			C.RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			C.RS.Stage_Texture	("$base1",		L_textures	);
			C.RS.Stage_Matrix		("$null",		L_matrices,	1);
			C.RS.Stage_Constant	("$null",		L_constants	);
		}
		C.RS.StageEnd			();
	}
	C.RS.PassEnd			();
	
	// Pass2 - Base map
	C.RS.PassBegin		();
	{
		C.RS.PassSET_ZB		(TRUE,FALSE);
		C.RS.PassSET_Blend_MUL();
		C.RS.PassSET_LightFog	(FALSE,TRUE);
		
		// Stage0 - Detail
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.RS.Stage_Texture	(oT_Name,		L_textures	);
			C.RS.Stage_Matrix		(oT_xform,		L_matrices,	0);
			C.RS.Stage_Constant	("$null",		L_constants	);
		}
		C.RS.StageEnd			();
	}
	C.RS.PassEnd			();
}
//
void CBlender_LaEmB::compile_3	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param)
{
	C.RS.PassBegin		();
	{
		C.RS.PassSET_ZB		(TRUE,TRUE);
		C.RS.PassSET_Blend_SET();
		C.RS.PassSET_LightFog	(FALSE,TRUE);
		
		// Stage0 - [=] Lightmap
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.RS.Stage_Texture	("$base1",		L_textures	);
			C.RS.Stage_Matrix		("$null",		L_matrices, 1);
			C.RS.Stage_Constant	("$null",		L_constants	);
		}
		C.RS.StageEnd			();
		
		// Stage1 - [+] Env-map
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			C.RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			C.RS.Stage_Texture	(oT2_Name,		L_textures	);
			C.RS.Stage_Matrix		(oT2_xform,		L_matrices,	0);
			C.RS.Stage_Constant	("$null",		L_constants	);
		}
		C.RS.StageEnd			();
		
		// Stage2 - [*] Base
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
			C.RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
			C.RS.Stage_Texture	(oT_Name,		L_textures	);
			C.RS.Stage_Matrix		(oT_xform,		L_matrices,	0);
			C.RS.Stage_Constant	("$null",		L_constants	);
		}
		C.RS.StageEnd			();
	}
	C.RS.PassEnd			();
}
//
void CBlender_LaEmB::compile_3c	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param)
{
	C.RS.PassBegin		();
	{
		C.RS.PassSET_ZB		(TRUE,TRUE);
		C.RS.PassSET_Blend_SET();
		C.RS.PassSET_LightFog	(FALSE,TRUE);
		
		// Stage1 - [=] Env-map [*] const
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			C.RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			C.RS.Stage_Texture	(oT2_Name,		L_textures	);
			C.RS.Stage_Matrix		(oT2_xform,		L_matrices,	0);
			C.RS.Stage_Constant	(oT2_const,		L_constants	);
		}
		C.RS.StageEnd			();
		
		// Stage0 - [=] Lightmap
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			C.RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			C.RS.Stage_Texture	("$base1",		L_textures	);
			C.RS.Stage_Matrix		("$null",		L_matrices, 1);
			C.RS.Stage_Constant	("$null",		L_constants	);
		}
		C.RS.StageEnd			();
		
		// Stage2 - [*] Base
		C.RS.StageBegin		();
		{
			C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
			C.RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
			C.RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
			C.RS.Stage_Texture	(oT_Name,		L_textures	);
			C.RS.Stage_Matrix		(oT_xform,		L_matrices,	0);
			C.RS.Stage_Constant	("$null",		L_constants	);
		}
		C.RS.StageEnd			();
	}
	C.RS.PassEnd			();
}