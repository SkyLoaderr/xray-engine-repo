// Blender_Vertex_aref.cpp: implementation of the CBlender_Tree class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Blender_tree.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Tree::CBlender_Tree()
{
	description.CLS		= B_TREE;
	description.version	= 0;
}

CBlender_Tree::~CBlender_Tree()
{

}

void	CBlender_Tree::Save		(IWriter& FS )
{
	CBlender::Save		(FS);
	xrPWRITE_PROP		(FS,"Alpha-blend",	xrPID_BOOL,		oBlend);
}

void	CBlender_Tree::Load		(IReader& FS, WORD version )
{
	CBlender::Load		(FS,version);
	xrPREAD_PROP		(FS,xrPID_BOOL,		oBlend);
}

#if RENDER==R_R1
//////////////////////////////////////////////////////////////////////////
// R1
//////////////////////////////////////////////////////////////////////////
void	CBlender_Tree::Compile	(CBlender_Compile& C)
{
	CBlender::Compile	(C);
	
	if (C.bEditor)
	{
		C.PassBegin		();
		{
			C.PassSET_ZB		(TRUE,TRUE);
			if (oBlend.value)	C.PassSET_Blend_BLEND	(TRUE, 200);
			else				C.PassSET_Blend_SET		(TRUE, 200);
			C.PassSET_LightFog	(TRUE,TRUE);
			
			// Stage1 - Base texture
			C.StageBegin		();
			C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,	D3DTA_DIFFUSE);
			C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,	D3DTA_DIFFUSE);
			C.StageSET_TMC		(oT_Name,"$null","$null",0);
			C.StageEnd			();
		}
		C.PassEnd			();
	} else {
		if (C.bLighting)	
		{
			C.PassBegin		();
			{
				C.PassSET_ZB		(TRUE,TRUE	);
				if (oBlend.value)	C.PassSET_Blend_BLEND	(TRUE, 200);
				else				C.PassSET_Blend_SET		(TRUE, 200);
				C.PassSET_LightFog	(FALSE,FALSE);
				C.PassSET_VS		("r1_tree_wave");
				
				// Stage1 - Base texture
				C.StageBegin		();
				C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,	D3DTA_DIFFUSE);
				C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				C.StageSET_TMC		(oT_Name,"$null","$null",0);
				C.StageEnd			();
			}
			C.PassEnd			();
		} else {
			C.PassBegin		();
			{
				C.PassSET_ZB		(TRUE,TRUE);
				if (oBlend.value)	C.PassSET_Blend_BLEND	(TRUE, 200);
				else				C.PassSET_Blend_SET		(TRUE, 200);
				C.PassSET_LightFog	(FALSE,TRUE);
				C.PassSET_VS		("r1_tree_wave");

				// Stage1 - Base texture
				C.StageBegin		();
				C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_DIFFUSE);
				C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_DIFFUSE);
				C.StageSET_TMC		(oT_Name,"$null","$null",0);
				C.StageEnd			();
			}
			C.PassEnd			();
		}
	}
}
#else
//////////////////////////////////////////////////////////////////////////
// R2
//////////////////////////////////////////////////////////////////////////
void	CBlender_Tree::Compile	(CBlender_Compile& C)
{
	CBlender::Compile	(C);

	if (C.bLighting)	
	{
	} else {
		if (oBlend.value)		C.r2_Pass				("r2_deffer_tree_flat","r2_deffer_base_aref_flat");
		else					C.r2_Pass				("r2_deffer_tree_flat","r2_deffer_base_flat");
		C.r2_Sampler			("s_base",C.L_textures[0]);
		C.r2_End				();
	}
}
#endif
