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

void	CBlender_LaEmB::Load(	CStream& FS )
{
	CBlender::Load	(FS);
	xrPREAD_MARKER	(FS);
	xrPREAD_PROP	(FS,xrPID_TEXTURE,	oT2_Name);
	xrPREAD_PROP	(FS,xrPID_MATRIX,	oT2_xform);
	xrPREAD_PROP	(FS,xrPID_CONSTANT,	oT2_const);
}

void	CBlender_LaEmB::Compile(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param, BOOL bEditor)
{
	if (bEditor)	{
		if (0==stricmp(oT2_const,"$null"))
		{
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
					RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_DIFFUSE);
					RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_DIFFUSE);
					RS.Stage_Texture	(oT2_Name,		L_textures		);
					RS.Stage_Matrix		(oT2_xform,		L_matrices,		0);
					RS.Stage_Constant	("$null",		L_constants		);
				}
				RS.StageEnd			();
				
				// Stage2 - Base texture
				RS.StageBegin		();
				{
					RS.StageSET_Address	(D3DTADDRESS_WRAP);
					RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
					RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
					RS.Stage_Texture	(oT_Name,		L_textures		);
					RS.Stage_Matrix		(oT_xform,		L_matrices,		0);
					RS.Stage_Constant	("$null",		L_constants		);
				}
				RS.StageEnd			();
			}
			RS.PassEnd			();
		} else {
			// WITH CONSTANT
			
			// Pass0 - (lmap+env*const)
			RS.PassBegin		();
			{
				RS.PassSET_ZB		(TRUE,TRUE);
				RS.PassSET_Blend_SET();
				RS.R().SetRS		(D3DRS_LIGHTING,					BC(TRUE));
				RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
				
				// Stage1 - Env texture * constant
				RS.StageBegin		();
				{
					RS.StageSET_Address	(D3DTADDRESS_WRAP);
					RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
					RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
					RS.Stage_Texture	(oT2_Name,		L_textures		);
					RS.Stage_Matrix		(oT2_xform,		L_matrices,		0);
					RS.Stage_Constant	(oT2_const,		L_constants		);
				}
				RS.StageEnd			();
				
				// Stage2 - Diffuse color
				RS.StageBegin		();
				{
					RS.StageSET_Address	(D3DTADDRESS_WRAP);
					RS.StageSET_Color	(D3DTA_DIFFUSE,	  D3DTOP_ADD,			D3DTA_CURRENT);
					RS.StageSET_Alpha	(D3DTA_DIFFUSE,	  D3DTOP_ADD,			D3DTA_CURRENT);
					RS.Stage_Texture	("$null",		L_textures		);
					RS.Stage_Matrix		("$null",		L_matrices,		0);
					RS.Stage_Constant	("$null",		L_constants		);
				}
				RS.StageEnd			();
			}
			RS.PassEnd			();
			
			// Pass1 - *base
			RS.PassBegin		();
			{
				RS.PassSET_ZB		(TRUE,FALSE);
				RS.PassSET_Blend_MUL();
				RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
				RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
				
				// Stage2 - Diffuse color
				RS.StageBegin		();
				{
					RS.StageSET_Address	(D3DTADDRESS_WRAP);
					RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
					RS.Stage_Texture	(oT_Name,		L_textures		);
					RS.Stage_Matrix		(oT_xform,		L_matrices,		0);
					RS.Stage_Constant	("$null",		L_constants		);
				}
				RS.StageEnd			();
			}
			RS.PassEnd			();
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
	RS.PassBegin		();
	{
		RS.PassSET_ZB		(TRUE,TRUE);
		RS.PassSET_Blend_SET();
		RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
		RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
		
		// Stage0 - Lightmap
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			RS.Stage_Texture	("$base1",		L_textures	);
			RS.Stage_Matrix		("$null",		L_matrices,	1);
			RS.Stage_Constant	("$null",		L_constants	);
		}
		RS.StageEnd			();
		
		// Stage1 - Environment map
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			RS.Stage_Texture	(oT2_Name,		L_textures	);
			RS.Stage_Matrix		(oT2_xform,		L_matrices,	0);
			RS.Stage_Constant	("$null",		L_constants	);
		}
		RS.StageEnd			();
	}
	RS.PassEnd			();
	
	// Pass2 - Base map
	RS.PassBegin		();
	{
		RS.PassSET_ZB		(TRUE,FALSE);
		RS.PassSET_Blend_MUL();
		RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
		RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
		
		// Stage0 - Detail
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			RS.Stage_Texture	(oT_Name,		L_textures	);
			RS.Stage_Matrix		(oT_xform,		L_matrices,	0);
			RS.Stage_Constant	("$null",		L_constants	);
		}
		RS.StageEnd			();
	}
	RS.PassEnd			();
}
//
void CBlender_LaEmB::compile_2c	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param)
{
	RS.PassBegin		();
	{
		RS.PassSET_ZB		(TRUE,TRUE);
		RS.PassSET_Blend_SET();
		RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
		RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
		
		// Stage0 - Environment map [*] const
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			RS.Stage_Texture	(oT2_Name,		L_textures	);
			RS.Stage_Matrix		(oT2_xform,		L_matrices,	0);
			RS.Stage_Constant	(oT2_const,		L_constants	);
		}
		RS.StageEnd			();

		// Stage1 - [+] Lightmap
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			RS.Stage_Texture	("$base1",		L_textures	);
			RS.Stage_Matrix		("$null",		L_matrices,	1);
			RS.Stage_Constant	("$null",		L_constants	);
		}
		RS.StageEnd			();
	}
	RS.PassEnd			();
	
	// Pass2 - Base map
	RS.PassBegin		();
	{
		RS.PassSET_ZB		(TRUE,FALSE);
		RS.PassSET_Blend_MUL();
		RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE)	);
		RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE)	);
		
		// Stage0 - Detail
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			RS.Stage_Texture	(oT_Name,		L_textures	);
			RS.Stage_Matrix		(oT_xform,		L_matrices,	0);
			RS.Stage_Constant	("$null",		L_constants	);
		}
		RS.StageEnd			();
	}
	RS.PassEnd			();
}
//
void CBlender_LaEmB::compile_3	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param)
{
	RS.PassBegin		();
	{
		RS.PassSET_ZB		(TRUE,TRUE);
		RS.PassSET_Blend_SET();
		RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
		RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
		
		// Stage0 - [=] Lightmap
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			RS.Stage_Texture	("$base1",		L_textures	);
			RS.Stage_Matrix		("$null",		L_matrices, 1);
			RS.Stage_Constant	("$null",		L_constants	);
		}
		RS.StageEnd			();
		
		// Stage1 - [+] Env-map
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			RS.Stage_Texture	(oT2_Name,		L_textures	);
			RS.Stage_Matrix		(oT2_xform,		L_matrices,	0);
			RS.Stage_Constant	("$null",		L_constants	);
		}
		RS.StageEnd			();
		
		// Stage2 - [*] Base
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
			RS.Stage_Texture	(oT_Name,		L_textures	);
			RS.Stage_Matrix		(oT_xform,		L_matrices,	0);
			RS.Stage_Constant	("$null",		L_constants	);
		}
		RS.StageEnd			();
	}
	RS.PassEnd			();
}
//
void CBlender_LaEmB::compile_3c	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param)
{
	RS.PassBegin		();
	{
		RS.PassSET_ZB		(TRUE,TRUE);
		RS.PassSET_Blend_SET();
		RS.R().SetRS		(D3DRS_LIGHTING,					BC(FALSE));
		RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));
		
		// Stage1 - [=] Env-map [*] const
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_TFACTOR);
			RS.Stage_Texture	(oT2_Name,		L_textures	);
			RS.Stage_Matrix		(oT2_xform,		L_matrices,	0);
			RS.Stage_Constant	(oT2_const,		L_constants	);
		}
		RS.StageEnd			();
		
		// Stage0 - [=] Lightmap
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_CURRENT);
			RS.Stage_Texture	("$base1",		L_textures	);
			RS.Stage_Matrix		("$null",		L_matrices, 1);
			RS.Stage_Constant	("$null",		L_constants	);
		}
		RS.StageEnd			();
		
		// Stage2 - [*] Base
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_CURRENT);
			RS.Stage_Texture	(oT_Name,		L_textures	);
			RS.Stage_Matrix		(oT_xform,		L_matrices,	0);
			RS.Stage_Constant	("$null",		L_constants	);
		}
		RS.StageEnd			();
	}
	RS.PassEnd			();
}