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

void	CBlender_Vertex_aref::Save(	IWriter& fs )
{
	IBlender::Save		(fs);
	xrPWRITE_PROP		(fs,"Alpha ref",	xrPID_INTEGER,	oAREF);
	xrPWRITE_PROP		(fs,"Alpha-blend",	xrPID_BOOL,		oBlend);
}

void	CBlender_Vertex_aref::Load(	IReader& fs, u16 version )
{
	IBlender::Load		(fs,version);

	switch (version)	
	{
	case 0: 
		xrPREAD_PROP	(fs,xrPID_INTEGER,	oAREF);
		oBlend.value	= FALSE;
		break;
	case 1:
	default:
		xrPREAD_PROP	(fs,xrPID_INTEGER,	oAREF);
		xrPREAD_PROP	(fs,xrPID_BOOL,		oBlend);
		break;
	}
}

void	CBlender_Vertex_aref::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);
	
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
		switch (C.iElement)
		{
		case SE_R1_NORMAL_HQ:
		case SE_R1_NORMAL_LQ:
			// Level view
			{
				LPCSTR			sname	= "r1_vert";
				if (C.bDetail)	sname	= "r1_vert_dt";
				if (oBlend.value)	C.r_Pass	(sname,sname,TRUE,TRUE,TRUE,TRUE,D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA,	TRUE,oAREF.value);
				else				C.r_Pass	(sname,sname,TRUE,TRUE,TRUE,TRUE,D3DBLEND_ONE,		D3DBLEND_ZERO,			TRUE,oAREF.value);
				C.r_Sampler		("s_base",	C.L_textures[0]);
				C.r_Sampler		("s_detail",C.detail_texture);
				C.r_End			();
			}
			break;
		case SE_R1_LPOINT:
			break;
		case SE_R1_LSPOT:
			C.r_Pass		("r1_vert_spot","r1_add_spot",FALSE,TRUE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE,TRUE,oAREF.value);
			C.r_Sampler		("s_base",	C.L_textures[0]);
			C.r_Sampler_clf	("s_lmap",	"internal\\internal_light_att");
			C.r_Sampler_clf	("s_att",	"internal\\internal_light_attclip");
			C.r_Constant	("L_dynamic_pos",	&RImplementation.r1_dlight_binder_PR);
			C.r_Constant	("L_dynamic_color",	&RImplementation.r1_dlight_binder_color);
			C.r_Constant	("L_dynamic_xform",	&RImplementation.r1_dlight_binder_xform);
			C.r_End			();
			break;
		case SE_R1_LMODELS:
			// Lighting only
			C.r_Pass		("r1_vert_l","r1_vert_l",FALSE);
			C.r_Sampler		("s_base",C.L_textures[0]);
			C.r_End			();
			break;
		}
	}
}
