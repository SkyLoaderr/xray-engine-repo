#include "stdafx.h"
#pragma hdrstop

#include "Blender_Screen_SET.h"

#define		VER_2_oBlendCount	7

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Screen_SET::CBlender_Screen_SET()
{
	description.CLS		= B_SCREEN_SET;
	description.version	= 3;
	oBlend.Count		= VER_2_oBlendCount;
	oBlend.IDselected	= 0;
	oAREF.value			= 32;
	oAREF.min			= 0;
	oAREF.max			= 255;
	oZTest.value		= FALSE;
	oZWrite.value		= FALSE;
	oLighting.value		= FALSE;
	oFog.value			= FALSE;
	oClamp.value		= TRUE;
}

CBlender_Screen_SET::~CBlender_Screen_SET()
{
	
}

void	CBlender_Screen_SET::Save	( CFS_Base& FS	)
{
	CBlender::Save	(FS);

	// Blend mode
	xrP_TOKEN::Item	I;
	xrPWRITE_PROP		(FS,"Blending",	xrPID_TOKEN,     oBlend);
	I.ID = 0; strcpy(I.str,"SET");		FS.write	(&I,sizeof(I));
	I.ID = 1; strcpy(I.str,"BLEND");	FS.write	(&I,sizeof(I));
	I.ID = 2; strcpy(I.str,"ADD");		FS.write	(&I,sizeof(I));
	I.ID = 3; strcpy(I.str,"MUL");		FS.write	(&I,sizeof(I));
	I.ID = 4; strcpy(I.str,"MUL_2X");	FS.write	(&I,sizeof(I));
	I.ID = 5; strcpy(I.str,"ALPHA-ADD");FS.write	(&I,sizeof(I));
	I.ID = 6; strcpy(I.str,"MUL_2X (B^D)");FS.write	(&I,sizeof(I));
	
	// Params
	xrPWRITE_PROP		(FS,"Texture clamp",xrPID_BOOL,		oClamp);
	xrPWRITE_PROP		(FS,"Alpha ref",	xrPID_INTEGER,	oAREF);
	xrPWRITE_PROP		(FS,"Z-test",		xrPID_BOOL,		oZTest);
	xrPWRITE_PROP		(FS,"Z-write",		xrPID_BOOL,		oZWrite);
	xrPWRITE_PROP		(FS,"Lighting",		xrPID_BOOL,		oLighting);
	xrPWRITE_PROP		(FS,"Fog",			xrPID_BOOL,		oFog);
}

void	CBlender_Screen_SET::Load	( CStream& FS, WORD version)
{
	CBlender::Load		(FS,version);

	switch (version)	{
	case 2:
		xrPREAD_PROP		(FS,xrPID_TOKEN,		oBlend);	oBlend.Count =   VER_2_oBlendCount;
		xrPREAD_PROP		(FS,xrPID_INTEGER,		oAREF);
		xrPREAD_PROP		(FS,xrPID_BOOL,			oZTest);
		xrPREAD_PROP		(FS,xrPID_BOOL,			oZWrite);
		xrPREAD_PROP		(FS,xrPID_BOOL,			oLighting);
		xrPREAD_PROP		(FS,xrPID_BOOL,			oFog);
		break;
	default:
		xrPREAD_PROP		(FS,xrPID_TOKEN,		oBlend);	oBlend.Count =   VER_2_oBlendCount;
		xrPREAD_PROP		(FS,xrPID_BOOL,			oClamp);
		xrPREAD_PROP		(FS,xrPID_INTEGER,		oAREF);
		xrPREAD_PROP		(FS,xrPID_BOOL,			oZTest);
		xrPREAD_PROP		(FS,xrPID_BOOL,			oZWrite);
		xrPREAD_PROP		(FS,xrPID_BOOL,			oLighting);
		xrPREAD_PROP		(FS,xrPID_BOOL,			oFog);
	}
}

void	CBlender_Screen_SET::Compile	(CBlender_Compile& C)
{
	CBlender::Compile		(C);
	C.PassBegin		();
	{
		C.PassSET_ZB		(oZTest.value,oZWrite.value);
		switch (oBlend.IDselected)
		{
		case 0:	// SET
			C.PassSET_Blend	(FALSE,	D3DBLEND_ONE,D3DBLEND_ZERO,				FALSE,0);
			break;
		case 1: // BLEND
			C.PassSET_Blend	(TRUE,	D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,	TRUE,oAREF.value);
			break;
		case 2:	// ADD
			C.PassSET_Blend	(TRUE,	D3DBLEND_ONE,D3DBLEND_ONE,				FALSE,oAREF.value);
			break;
		case 3:	// MUL
			C.PassSET_Blend	(TRUE,	D3DBLEND_DESTCOLOR,D3DBLEND_ZERO,		FALSE,oAREF.value);
			break;
		case 4:	// MUL_2X
			C.PassSET_Blend	(TRUE,	D3DBLEND_DESTCOLOR,D3DBLEND_SRCCOLOR,	FALSE,oAREF.value);
			break;
		case 5:	// ALPHA-ADD
			C.PassSET_Blend	(TRUE,	D3DBLEND_SRCALPHA,D3DBLEND_ONE,			TRUE,oAREF.value);
			break;
		case 6:	// MUL_2X + A-test
			C.PassSET_Blend	(TRUE,	D3DBLEND_DESTCOLOR,D3DBLEND_SRCCOLOR,	TRUE,oAREF.value);
			break;
		}
		C.PassSET_LightFog	(oLighting.value,oFog.value);

		if (oBlend.IDselected==6)	
		{
			// Usually for wallmarks
			C.StageBegin		();
			C.StageSET_Address	(oClamp.value?D3DTADDRESS_CLAMP:D3DTADDRESS_WRAP);
			C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.Stage_Texture		(oT_Name);
			C.Stage_Matrix		("$null",0);
			C.Stage_Constant	("$null");
			C.StageEnd			();

			C.StageBegin		();
			C.StageSET_Address	(oClamp.value?D3DTADDRESS_CLAMP:D3DTADDRESS_WRAP);
			C.StageSET_Color	(D3DTA_DIFFUSE,	  D3DTOP_BLENDDIFFUSEALPHA,	D3DTA_CURRENT);
			C.StageSET_Alpha	(D3DTA_DIFFUSE,	  D3DTOP_SELECTARG2,		D3DTA_CURRENT);
			C.Stage_Texture		("$null");
			C.Stage_Matrix		("$null",	0);
			C.Stage_Constant	("$null");
			C.StageEnd			();
		} else {
			C.StageBegin		();
			C.StageSET_Address	(oClamp.value?D3DTADDRESS_CLAMP:D3DTADDRESS_WRAP);
			C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			C.Stage_Texture		(oT_Name);
			C.Stage_Matrix		(oT_xform,	0);
			C.Stage_Constant	("$null");
			C.StageEnd			();
		}
	}
	C.PassEnd			();
}
