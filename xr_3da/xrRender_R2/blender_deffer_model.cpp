#include "stdafx.h"
#pragma hdrstop

#include "uber_deffer.h"
#include "Blender_deffer_model.h"

CBlender_deffer_model::CBlender_deffer_model	()	{	
	description.CLS		= B_MODEL;	
	description.version	= 1;
	oAREF.value			= 32;
	oAREF.min			= 0;
	oAREF.max			= 255;
	oBlend.value		= FALSE;
}
CBlender_deffer_model::~CBlender_deffer_model	()	{	}

void	CBlender_deffer_model::Save	(	IWriter& fs )
{
	IBlender::Save		(fs);
	xrPWRITE_PROP		(fs,"Use alpha-channel",	xrPID_BOOL,		oBlend);
	xrPWRITE_PROP		(fs,"Alpha ref",			xrPID_INTEGER,	oAREF);
}
void	CBlender_deffer_model::Load	(	IReader& fs, u16 version )
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

void	CBlender_deffer_model::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	if (oBlend.value || oStrictSorting.value)		{
		// forward rendering
		LPCSTR	vsname,psname;
		switch(C.iElement) 
		{
		case 0: 	//
		case 1: 	//
			vsname = psname =	"model_def_lq"; 
			C.r_Pass			(vsname,psname,TRUE,TRUE,TRUE,TRUE,D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA,	TRUE,oAREF.value);
			C.r_Sampler			("s_base",	C.L_textures[0]);
			C.r_End				();
			break;
		default:
			break;
		}
	} else {
		// deferred rendering
		// codepath is the same, only the shaders differ
		switch(C.iElement) 
		{
		case 0: 	// deffer
			uber_deffer		(C,"model","base",false);
			break;
		case CRender::PHASE_SMAP_D:	// smap-direct
		case CRender::PHASE_SMAP_P:	// smap-point
		case CRender::PHASE_SMAP_S:	// smap-spot							//. !!!! dumb
			if (RImplementation.o.HW_smap)	C.r_Pass	("shadow_direct_model","dumb",					FALSE,TRUE,TRUE,FALSE);
			else							C.r_Pass	("shadow_direct_model","shadow_direct_base",	FALSE);
			C.r_Sampler		("s_base",		C.L_textures[0]);
			C.r_End			();
			break;
		}
	}
}
