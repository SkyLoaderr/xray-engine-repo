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

void	CBlender_Model::Save	( CFS_Base& FS	)
{
	CBlender::Save	(FS);
}

void	CBlender_Model::Load	( CStream& FS, WORD version)
{
	CBlender::Load		(FS,version);
}

void	CBlender_Model::Compile	(CBlender_Compile& C)
{
	CBlender::Compile		(C);
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
			C.StageSET_TMC		(oT_Name, "$null", "$null", 0			);
			C.StageEnd			();
		}
		C.PassEnd			();
	} else {
		C.PassBegin		();
		{
			C.PassSET_ZB		(TRUE,TRUE);
			C.PassSET_Blend_SET	();
			C.PassSET_LightFog	(TRUE,TRUE);

			switch (C.iLOD)	
			{
			case 0:	// Highest LOD
				C.StageBegin		();
				C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_ADD,			D3DTA_DIFFUSE);
				C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				C.StageSET_TMC		("$user$projector", "$user$projector", "$null", 0	);
				C.StageEnd			();

				C.StageBegin		();
				C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_CURRENT);
				C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_CURRENT);
				C.StageSET_TMC		(oT_Name, "$null", "$null", 0			);
				C.StageEnd			();
				break;
			case 1:	// Lowest LOD
				C.StageBegin		();
				C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE2X,	D3DTA_DIFFUSE);
				C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
				C.StageSET_TMC		(oT_Name, "$null", "$null", 0			);
				C.StageEnd			();
				break;
			}
		}
		C.PassEnd			();
	}
}
