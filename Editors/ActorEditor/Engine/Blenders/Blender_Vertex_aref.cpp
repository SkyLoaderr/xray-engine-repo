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
	description.version	= 1;
	oAREF.value			= 32;
	oAREF.min			= 0;
	oAREF.max			= 255;
	oBlend.value		= FALSE;
}

CBlender_Vertex_aref::~CBlender_Vertex_aref()
{

}

void	CBlender_Vertex_aref::Save(	IWriter& FS )
{
	CBlender::Save		(FS);
	xrPWRITE_PROP		(FS,"Alpha ref",	xrPID_INTEGER,	oAREF);
	xrPWRITE_PROP		(FS,"Alpha-blend",	xrPID_BOOL,		oBlend);
}

void	CBlender_Vertex_aref::Load(	IReader& FS, WORD version )
{
	CBlender::Load		(FS,version);

	switch (version)	
	{
	case 0: 
		xrPREAD_PROP	(FS,xrPID_INTEGER,	oAREF);
		oBlend.value	= FALSE;
		break;
	case 1:
	default:
		xrPREAD_PROP	(FS,xrPID_INTEGER,	oAREF);
		xrPREAD_PROP	(FS,xrPID_BOOL,		oBlend);
		break;
	}
}

void	CBlender_Vertex_aref::Compile(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	
	if (C.bEditor)
	{
		C.PassBegin		();
		{
			C.PassSET_ZB		(TRUE,TRUE);
			if (oBlend.value)	C.PassSET_Blend			(TRUE, D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,	TRUE,oAREF.value);
			else				C.PassSET_Blend			(TRUE, D3DBLEND_ONE, D3DBLEND_ZERO,				TRUE,oAREF.value);
			C.PassSET_LightFog	(TRUE,TRUE);
			
			// Stage1 - Base texture
			C.StageBegin		();
			C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,	D3DTA_DIFFUSE);
			C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,	D3DTA_DIFFUSE);
			C.Stage_Texture		(oT_Name);
			C.Stage_Matrix		(oT_xform,	0);
			C.Stage_Constant	("$null");
			C.StageEnd			();
		}
		C.PassEnd			();
	} else {
		if (2==C.iElement)	
		{
			C.PassBegin		();
			{
				C.PassSET_ZB		(TRUE,TRUE	);
				if (oBlend.value)	C.PassSET_Blend			(TRUE, D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,	TRUE,oAREF.value);
				else				C.PassSET_Blend			(TRUE, D3DBLEND_ONE, D3DBLEND_ZERO,				TRUE,oAREF.value);
				C.PassSET_LightFog	(FALSE,FALSE);
				
				// Stage1 - Base texture
				C.StageBegin		();
				C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_DIFFUSE);
				C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				C.Stage_Texture		(oT_Name);
				C.Stage_Matrix		(oT_xform,0);
				C.Stage_Constant	("$null");
				C.StageEnd			();
			}
			C.PassEnd			();
		} else {
			C.PassBegin		();
			{
				C.PassSET_ZB		(TRUE,TRUE);
				if (oBlend.value)	C.PassSET_Blend			(TRUE, D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,	TRUE,oAREF.value);
				else				C.PassSET_Blend			(TRUE, D3DBLEND_ONE, D3DBLEND_ZERO,				TRUE,oAREF.value);
				C.PassSET_LightFog	(C.bEditor,TRUE);
				
				// Stage1 - Base texture
				C.StageBegin		();
				C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_DIFFUSE);
				C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_DIFFUSE);
				C.Stage_Texture		(oT_Name);
				C.Stage_Matrix		(oT_xform,	0);
				C.Stage_Constant	("$null");
				C.StageEnd			();
			}
			C.PassEnd			();
		}
	}
}
