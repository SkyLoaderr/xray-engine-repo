// Blender_Vertex_aref.cpp: implementation of the CBlender_Vertex_aref class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Blender_Vertex_aref.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Vertex_aref::CBlender_Vertex_aref()
{
	description.CLS		= B_VERT_AREF;
	oAREF.value			= 32;
	oAREF.min			= 0;
	oAREF.max			= 255;
}

CBlender_Vertex_aref::~CBlender_Vertex_aref()
{

}

void	CBlender_Vertex_aref::Save(	CFS_Base& FS )
{
	CBlender::Save		(FS);
	xrPWRITE_PROP		(FS,"Alpha ref",	xrPID_INTEGER,	oAREF);
}

void	CBlender_Vertex_aref::Load(	CStream& FS, WORD version )
{
	CBlender::Load		(FS,version);
	xrPREAD_PROP		(FS,xrPID_INTEGER,	oAREF);
}

void	CBlender_Vertex_aref::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	
	if (C.bLighting)	
	{
		C.RS.PassBegin		();
		{
			C.RS.PassSET_ZB			(TRUE,TRUE	);
			C.RS.PassSET_Blend		(TRUE, D3DBLEND_ONE, D3DBLEND_ZERO,TRUE,oAREF.value);
			C.RS.PassSET_LightFog	(FALSE,FALSE);
			
			// Stage1 - Base texture
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
				C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_DIFFUSE);
				C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				C.RS.Stage_Texture		(oT_Name,	C.L_textures);
				C.RS.Stage_Matrix		(oT_xform,	C.L_matrices,	0);
				C.RS.Stage_Constant		("$null",	C.L_constants);
			}
			C.RS.StageEnd			();
		}
		C.RS.PassEnd			();
	} else {
		C.RS.PassBegin		();
		{
			C.RS.PassSET_ZB			(TRUE,TRUE);
			C.RS.PassSET_Blend		(TRUE, D3DBLEND_ONE, D3DBLEND_ZERO,TRUE,oAREF.value);
			C.RS.PassSET_LightFog	(C.bEditor,TRUE);
			
			// Stage1 - Base texture
			C.RS.StageBegin		();
			{
				C.RS.StageSET_Address	(D3DTADDRESS_WRAP);
				C.RS.StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				C.RS.StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
				C.RS.Stage_Texture		(oT_Name,	C.L_textures);
				C.RS.Stage_Matrix		(oT_xform,	C.L_matrices,	0);
				C.RS.Stage_Constant		("$null",	C.L_constants);
			}
			C.RS.StageEnd			();
		}
		C.RS.PassEnd			();
	}
}
