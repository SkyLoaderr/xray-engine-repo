#include "stdafx.h"
#pragma hdrstop

#include "Blender_Model.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Model::CBlender_Model()
{
	description.CLS		= B_MODEL;
	description.version	= 0;
}

CBlender_Model::~CBlender_Model()
{
	
}

void	CBlender_Model::Save	( IWriter& fs	)
{
	IBlender::Save	(fs);
}

void	CBlender_Model::Load	( IReader& fs, WORD version)
{
	IBlender::Load		(fs,version);
}

void	CBlender_Model::Compile	(CBlender_Compile& C)
{
	IBlender::Compile		(C);
	if (C.bEditor)
	{
		C.PassBegin		();
		{
			C.PassSET_ZB		(TRUE,TRUE);
			C.PassSET_Blend_SET	();
			C.PassSET_LightFog	(TRUE,TRUE);
			C.StageBegin		();
			C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.StageSET_TMC		(oT_Name,	"$null",	"$null",	0		);
			C.StageEnd			();
		}
		C.PassEnd			();
	} else {
		switch (C.iElement)	
		{
		case 0:	// Highest LOD
			C.r_Pass			("r1_model_def_hq","r1_model_def_hq",TRUE);
			C.r_Sampler			("s_base",	C.L_textures[0]);
			C.r_Sampler			("s_lmap",	"$user$projector"); //, D3DTADDRESS_CLAMP, D3DTEXF_LINEAR, D3DTEXF_NONE,D3DTEXF_LINEAR);
			C.r_End				();
			break;
		case 1:	// Lowest LOD
			C.PassBegin			();
			C.PassSET_ZB		(TRUE,TRUE);
			C.PassSET_Blend_SET	();
			C.PassSET_LightFog	(TRUE,TRUE);
			C.StageBegin		();
			C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_DIFFUSE);
			C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.StageSET_TMC		(oT_Name, "$null", "$null", 0			);
			C.StageEnd			();
			C.PassEnd			();
			break;
		}
	}
}
