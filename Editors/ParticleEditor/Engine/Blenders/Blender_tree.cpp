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

void	CBlender_Tree::Save		(IWriter& fs )
{
	IBlender::Save		(fs);
	xrPWRITE_PROP		(fs,"Alpha-blend",	xrPID_BOOL,		oBlend);
}

void	CBlender_Tree::Load		(IReader& fs, u16 version )
{
	IBlender::Load		(fs,version);
	xrPREAD_PROP		(fs,xrPID_BOOL,		oBlend);
}

#if RENDER==R_R1
//////////////////////////////////////////////////////////////////////////
// R1
//////////////////////////////////////////////////////////////////////////
void	CBlender_Tree::Compile	(CBlender_Compile& C)
{
	IBlender::Compile	(C);
	
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
		const u32			tree_aref		= 200;
		switch (C.iElement)
		{
		case SE_R1_NORMAL_HQ:
		case SE_R1_NORMAL_LQ:
			// Level view
			if (oBlend.value)	C.r_Pass	("tree_wave","vert",TRUE,TRUE,TRUE,TRUE,D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA,	TRUE,tree_aref);
			else				C.r_Pass	("tree_wave","vert",TRUE,TRUE,TRUE,TRUE,D3DBLEND_ONE,			D3DBLEND_ZERO,			TRUE,tree_aref);
			C.r_Sampler		("s_base",	C.L_textures[0]);
			C.r_Sampler		("s_detail",C.detail_texture);
			C.r_End			();
			break;
		case SE_R1_LPOINT:
			C.r_Pass		("tree_wave_point","add_point",FALSE,TRUE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE,TRUE,0);
			C.r_Sampler		("s_base",	C.L_textures[0]);
			C.r_Sampler_clf	("s_att",	TEX_POINT_ATT		);
			C.r_End			();
			break;
		case SE_R1_LSPOT:
			C.r_Pass		("tree_wave_spot","add_spot",FALSE,TRUE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE,TRUE,0);
			C.r_Sampler		("s_base",	C.L_textures[0]);
			C.r_Sampler_clf	("s_lmap",	"internal\\internal_light_att",		true);
			C.r_Sampler_clf	("s_att",	TEX_SPOT_ATT		);
			C.r_End			();
			break;
		case SE_R1_LMODELS:
			// Lighting only
			C.r_Pass		("tree_wave","vert_l",FALSE);
			C.r_Sampler		("s_base",C.L_textures[0]);
			C.r_End			();
			break;
		}
	}
}
#else
//////////////////////////////////////////////////////////////////////////
// R2
//////////////////////////////////////////////////////////////////////////
class cl_chpos	: public R_constant_setup 
{
	u32			dwFrame;
	Fvector4	c_hpos;

	virtual void setup (R_constant* C) 
	{ 
		if (Device.dwFrame != dwFrame)	
		{
			//dwFrame							= Device.dwFrame;
			RCache.xforms.m_vp.transform	(c_hpos,Device.vCameraPosition);
			c_hpos.mul						(1/c_hpos.w);
		}
		RCache.set_c	(C,c_hpos);	
	}
};
static cl_chpos	binder_chpos;

void	CBlender_Tree::Compile	(CBlender_Compile& C)
{
	IBlender::Compile	(C);

	//*************** codepath is the same, only shaders differ
	switch (C.iElement)
	{
	case 0:		// deffer
		if (oBlend.value)	C.r_Pass	("deffer_tree_flat","deffer_base_aref_flat",FALSE);
		else				C.r_Pass	("deffer_tree_flat","deffer_base_flat",FALSE);
		C.r_Sampler			("s_base",C.L_textures[0]);
		C.r_End				();
		break;
	case 1:		// smap-direct
		if (oBlend.value)	C.r_Pass	("shadow_direct_tree","shadow_direct_aref",FALSE,TRUE,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,TRUE,220);
		else				C.r_Pass	("shadow_direct_base","shadow_direct_base",FALSE);
		C.r_Sampler			("s_base",C.L_textures[0]);
		C.r_End				();
		break;
	case 2:		// smap-point
		if (oBlend.value)	C.r_Pass	("shadow_point_aref","shadow_point_aref",FALSE);
		else				C.r_Pass	("shadow_point_base","shadow_point_base",FALSE);
		C.r_Sampler			("s_base",C.L_textures[0]);
		C.r_Constant		("Ldynamic_pos",	&RImplementation.Binders.l_position);
		C.r_End				();
		break;
	case 3:		// smap-spot
		if (oBlend.value)	C.r_Pass	("shadow_direct_tree","shadow_direct_aref",FALSE,TRUE,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,TRUE,220);
		else				C.r_Pass	("shadow_direct_base","shadow_direct_base",FALSE);
		C.r_Sampler			("s_base",C.L_textures[0]);
		C.r_End				();
		break;
	}
}
#endif

