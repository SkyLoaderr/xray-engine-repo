#include "stdafx.h"
#pragma hdrstop

#include "Blender_Screen_SET.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Screen_SET::CBlender_Screen_SET()
{
	description.CLS		= B_SCREEN_SET;
	description.version	= 2;
	oBlend.Count		= 6;
	oBlend.IDselected	= 0;
	oAREF.value			= 32;
	oAREF.min			= 0;
	oAREF.max			= 255;
	oZTest.value		= FALSE;
	oZWrite.value		= FALSE;
	oLighting.value		= FALSE;
	oFog.value			= FALSE;
}

CBlender_Screen_SET::~CBlender_Screen_SET()
{
	
}

void	CBlender_Screen_SET::Save	( CFS_Base& FS	)
{
	CBlender::Save	(FS);

	// Blend mode
	xrP_TOKEN::Item	I;
	xrPWRITE_PROP		("Blending",	xrPID_TOKEN,     oBlend);
	I.ID = 0; strcpy(I.str,"SET");		FS.write	(&I,sizeof(I));
	I.ID = 1; strcpy(I.str,"BLEND");	FS.write	(&I,sizeof(I));
	I.ID = 2; strcpy(I.str,"ADD");		FS.write	(&I,sizeof(I));
	I.ID = 3; strcpy(I.str,"MUL");		FS.write	(&I,sizeof(I));
	I.ID = 4; strcpy(I.str,"MUL_2X");	FS.write	(&I,sizeof(I));
	I.ID = 5; strcpy(I.str,"ALPHA-ADD");FS.write	(&I,sizeof(I));
	
	// Params
	xrPWRITE_PROP		("Alpha ref",	xrPID_INTEGER,	oAREF);
	xrPWRITE_PROP		("Z-test",      xrPID_BOOL,		oZTest);
	xrPWRITE_PROP		("Z-write",     xrPID_BOOL,		oZWrite);
	xrPWRITE_PROP		("Lighting",    xrPID_BOOL,		oLighting);
	xrPWRITE_PROP		("Fog",			xrPID_BOOL,		oFog);
}

void	CBlender_Screen_SET::Load	( CStream& FS	)
{
	CBlender::Load	(FS);

	// Blend mode
	xrP_TOKEN::Item	I;
	xrPREAD_PROP			(xrPID_TOKEN,		oBlend);
	for (DWORD it=0; it<oBlend.Count; it++)	FS.Read	(&I,sizeof(I));
	
	// Params
	xrPREAD_PROP			(xrPID_INTEGER,		oAREF);
	xrPREAD_PROP			(xrPID_BOOL,		oZTest);
	xrPREAD_PROP			(xrPID_BOOL,		oZWrite);
	xrPREAD_PROP			(xrPID_BOOL,		oLighting);
	xrPREAD_PROP			(xrPID_BOOL,		oFog);
}

void	CBlender_Screen_SET::Compile	(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param, BOOL bEditor)
{
	RS.PassBegin		();
	{
		RS.PassSET_ZB		(oZTest.value,oZWrite.value);
		switch (oBlend.IDselected)
		{
		case 0:	// SET
			RS.PassSET_Blend	(FALSE,	D3DBLEND_ONE,D3DBLEND_ZERO,				FALSE,0);
			break;
		case 1: // BLEND
			RS.PassSET_Blend	(TRUE,	D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,	TRUE,oAREF.value);
			break;
		case 2:	// ADD
			RS.PassSET_Blend	(TRUE,	D3DBLEND_ONE,D3DBLEND_ONE,				FALSE,oAREF.value);
			break;
		case 3:	// MUL
			RS.PassSET_Blend	(TRUE,	D3DBLEND_DESTCOLOR,D3DBLEND_ZERO,		FALSE,oAREF.value);
			break;
		case 4:	// MUL_2X
			RS.PassSET_Blend	(TRUE,	D3DBLEND_DESTCOLOR,D3DBLEND_SRCCOLOR,	FALSE,oAREF.value);
			break;
		case 5:	// ALPHA-ADD
			RS.PassSET_Blend	(TRUE,	D3DBLEND_SRCALPHA,D3DBLEND_ONE,			FALSE,oAREF.value);
			break;
		}
		RS.R().SetRS		(D3DRS_LIGHTING,					BC(oLighting.value));
		RS.R().SetRS		(D3DRS_FOGENABLE,					BC(oFog.value));
		RS.StageBegin		();
		{
			RS.StageSET_Address	(D3DTADDRESS_CLAMP);
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
