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
		if (C.iElement==2)	
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
	CBlender::Compile	(C);

	//*************** codepath is the same, only shaders differ
	switch (C.iElement)
	{
	case 0:		// deffer
		if (oBlend.value)		C.r2_Pass				(r2v("r2_deffer_tree_flat"),r2p("r2_deffer_base_aref_flat"));
		else					C.r2_Pass				(r2v("r2_deffer_tree_flat"),r2p("r2_deffer_base_flat"));
		C.r2_Sampler			("s_base",C.L_textures[0]);
		C.r2_End				();
		break;
	case 1:		// smap-direct
		if (oBlend.value)		C.r2_Pass				(r2v("r2_shadow_direct_aref"),r2p("r2_shadow_direct_aref"));
		else					C.r2_Pass				(r2v("r2_shadow_direct_base"),r2p("r2_shadow_direct_base"));
		C.r2_Sampler			("s_base",C.L_textures[0]);
		C.r2_End				();
		break;
	case 2:		// smap-point
		if (oBlend.value)		C.r2_Pass				(r2v("r2_shadow_point_aref"),r2p("r2_shadow_point_aref"));
		else					C.r2_Pass				(r2v("r2_shadow_point_base"),r2p("r2_shadow_point_base"));
		C.r2_Sampler			("s_base",C.L_textures[0]);
		C.r2_Constant			("light_position",	&RImplementation.Binders.l_position);
		C.r2_End				();
		break;
	case 3:		// smap-spot
		/*if (oBlend.value)		C.r2_Pass				(r2v("r2_shadow_point_aref"),r2p("r2_shadow_point_aref"));
		else*/					C.r2_Pass				(r2v("r2_shadow_spot_base"),r2p("r2_shadow_direct_base"));
		C.r2_Sampler			("s_base",C.L_textures[0]);
		C.r2_End				();
		break;
	}
}
#endif
