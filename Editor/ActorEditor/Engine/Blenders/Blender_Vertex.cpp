// Blender_Vertex.cpp: implementation of the CBlender_Vertex class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Blender_Vertex.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Vertex::CBlender_Vertex()
{
	description.CLS		= B_VERT;
}

CBlender_Vertex::~CBlender_Vertex()
{
	
}

void	CBlender_Vertex::Save	( CFS_Base& FS	)
{
	CBlender::Save	(FS);
}

void	CBlender_Vertex::Load	( CStream& FS, WORD version	)
{
	CBlender::Load	(FS,version);
}

void CBlender_Vertex::Compile	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param, BOOL bEditor)
{
	RS.PassBegin		();
	{
		RS.PassSET_ZB		(TRUE,TRUE);
		RS.PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,	FALSE,0);
		RS.R().SetRS		(D3DRS_LIGHTING,					BC(bEditor?TRUE:FALSE));
		RS.R().SetRS		(D3DRS_FOGENABLE,					BC(TRUE));

		// Stage0 - Base texture
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_WRAP);
			RS.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			RS.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			RS.Stage_Texture	(oT_Name,	L_textures);
			RS.Stage_Matrix		(oT_xform,	L_matrices,	0);
			RS.Stage_Constant	("$null",	L_constants);
		}
		RS.StageEnd			();
	}
	RS.PassEnd			();
}
