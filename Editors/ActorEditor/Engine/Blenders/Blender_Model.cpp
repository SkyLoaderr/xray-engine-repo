#include "stdafx.h"
#pragma hdrstop

#include "Blender_Model.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Model::CBlender_Model()
{
	description.CLS		= B_MODEL;
	description.version	= 1;
	oAREF.value			= 32;
	oAREF.min			= 0;
	oAREF.max			= 255;
	oBlend.value		= FALSE;
}

CBlender_Model::~CBlender_Model()
{
	
}

void	CBlender_Model::Save	( IWriter& fs	)
{
	IBlender::Save		(fs);
	xrPWRITE_PROP		(fs,"Use alpha-channel",	xrPID_BOOL,		oBlend);
	xrPWRITE_PROP		(fs,"Alpha ref",			xrPID_INTEGER,	oAREF);
}

void	CBlender_Model::Load	( IReader& fs, u16 version)
{
	IBlender::Load		(fs,version);

	switch (version)	
	{
	case 0: 
		oAREF.value			= 32;
		oAREF.min			= 0;
		oAREF.max			= 255;
		oBlend.value		= FALSE;
		break;
	case 1:
	default:
		xrPREAD_PROP	(fs,xrPID_BOOL,		oBlend);
		xrPREAD_PROP	(fs,xrPID_INTEGER,	oAREF);
		break;
	}
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
		LPCSTR	sname		= 0;
		switch (C.iElement)	
		{
		case 0:	// Highest LOD
			sname				= "r1_model_def_hq"; 
			if (oBlend.value)	C.r_Pass	(sname,sname,TRUE,TRUE,TRUE,TRUE,D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA,	TRUE,oAREF.value);
			else				C.r_Pass	(sname,sname,TRUE);
			C.r_Sampler			("s_base",	C.L_textures[0]);
			C.r_Sampler			("s_lmap",	"$user$projector", D3DTADDRESS_CLAMP, D3DTEXF_LINEAR, D3DTEXF_NONE,D3DTEXF_LINEAR);
			C.r_End				();
			break;
		case 1:	// Lowest LOD
			sname				= "r1_model_def_lq"; 
			if (oBlend.value)	C.r_Pass	(sname,sname,TRUE,TRUE,TRUE,TRUE,D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA,	TRUE,oAREF.value);
			else				C.r_Pass	(sname,sname,TRUE);
			C.r_Sampler			("s_base",	C.L_textures[0]);
			C.r_End				();
			break;
		}
	}
}
