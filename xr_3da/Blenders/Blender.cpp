// Blender.cpp: implementation of the CBlender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Blender.h"

void CBlender_DESC::Setup	(LPCSTR N)
{
	// Name
	VERIFY(strlen(N)<128);
	VERIFY(0==strchr(N,'.'));
	strcpy(cName,N);
	strlwr(cName);
	
	// Computer
	const u32 comp = MAX_COMPUTERNAME_LENGTH + 1;
	char	buf	[comp];
	DWORD	sz = comp;
	GetComputerName(buf,&sz);
	if (sz > 31) sz=31;
	buf[sz] = 0;
	strcpy(cComputer,buf);
	
	// Time
	_tzset(); time( (long*)&cTime );
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender::CBlender()
{
	oPriority.min	= 0;
	oPriority.max	= 3;
	oPriority.value	= 1;
	strcpy			(oT_Name,	"$base0");
	strcpy			(oT_xform,	"$null");
}

CBlender::~CBlender()
{

}

void	CBlender::Save(	CFS_Base& FS )
{
	FS.write		(&description,sizeof(description));
	xrPWRITE_MARKER (FS,"General");
	xrPWRITE_PROP	(FS,"Priority",			xrPID_INTEGER,	oPriority);
	xrPWRITE_PROP	(FS,"Strict sorting",	xrPID_BOOL,		oStrictSorting);
	xrPWRITE_MARKER	(FS,"Base Texture");
	xrPWRITE_PROP	(FS,"Name",				xrPID_TEXTURE,	oT_Name);
	xrPWRITE_PROP	(FS,"Transform",		xrPID_MATRIX,	oT_xform);
}

void	CBlender::Load(	CStream& FS, WORD version )
{
	// Read desc and doesn't change version
	WORD	V		= description.version;
	FS.Read			(&description,sizeof(description));
	description.version	= V;

	// Properties
	xrPREAD_MARKER	(FS);
	xrPREAD_PROP	(FS,xrPID_INTEGER,	oPriority);
	xrPREAD_PROP	(FS,xrPID_BOOL,		oStrictSorting);
	xrPREAD_MARKER	(FS);
	xrPREAD_PROP	(FS,xrPID_TEXTURE,	oT_Name);
	xrPREAD_PROP	(FS,xrPID_MATRIX,	oT_xform);
}

void	CBlender::Compile(CBlender_Compile& C)
{
	if (C.bEditor)	C.SetParams	(oPriority.value,oStrictSorting.value?true:false,true,	false);
	else			C.SetParams	(oPriority.value,oStrictSorting.value?true:false,false,	false);
}

//////////////////////////////////////////////////////////////////////
#include "blender_clsid.h"
#include "blenderdefault.h"
#include "blender_default_aref.h"
#include "blender_vertex.h"
#include "blender_vertex_aref.h"
#include "blender_screen_set.h"
#include "blender_screen_gray.h"
#include "blender_editor_wire.h"
#include "blender_editor_selection.h"
#include "blender_light.h"
#include "blender_LaEmB.h"
#include "blender_Lm(EbB).h"
#include "blender_LmBmmD.h"
#include "blender_BmmD.h"
#include "blender_shadow_texture.h"
#include "blender_shadow_world.h"
#include "blender_blur.h"
#include "blender_model.h"
#include "blender_model_ebb.h"

CBlender*	CBlender::Create	(CLASS_ID cls)
{	
	switch (cls)
	{
	case B_DEFAULT:			return new CBlender_default;		break;
	case B_DEFAULT_AREF:	return new CBlender_default_aref;	break;
	case B_VERT:			return new CBlender_Vertex;			break;
	case B_VERT_AREF:		return new CBlender_Vertex_aref;	break;
	case B_SCREEN_SET:		return new CBlender_Screen_SET;		break;
	case B_SCREEN_GRAY:		return new CBlender_Screen_GRAY;	break;
	case B_EDITOR_WIRE:		return new CBlender_Editor_Wire;	break;
	case B_EDITOR_SEL:		return new CBlender_Editor_Selection;break;
	case B_LIGHT:			return new CBlender_LIGHT;			break;
	case B_LmBmmD:			return new CBlender_LmBmmD;			break;
	case B_LaEmB:			return new CBlender_LaEmB;			break;
	case B_LmEbB:			return new CBlender_LmEbB;			break;
	case B_BmmD:			return new CBlender_BmmD;			break;
	case B_SHADOW_TEX:		return new CBlender_ShTex;			break;
	case B_SHADOW_WORLD:	return new CBlender_ShWorld;		break;
	case B_BLUR:			return new CBlender_Blur;			break;
	case B_MODEL:			return new CBlender_Model;			break;
	case B_MODEL_EbB:		return new CBlender_Model_EbB;		break;
	default:				return 0;
	}
}

void		CBlender::CreatePalette(vector<CBlender*> &palette)
{
	R_ASSERT(palette.empty());
	palette.push_back(Create(B_DEFAULT));
	palette.push_back(Create(B_DEFAULT_AREF));
	palette.push_back(Create(B_VERT));
	palette.push_back(Create(B_VERT_AREF));
	palette.push_back(Create(B_SCREEN_SET));
	palette.push_back(Create(B_SCREEN_GRAY));
	palette.push_back(Create(B_EDITOR_WIRE));
	palette.push_back(Create(B_EDITOR_SEL));
	palette.push_back(Create(B_LIGHT));
	palette.push_back(Create(B_LmBmmD));
	palette.push_back(Create(B_LaEmB));
	palette.push_back(Create(B_LmEbB));
	palette.push_back(Create(B_BmmD));
	palette.push_back(Create(B_SHADOW_TEX));
	palette.push_back(Create(B_SHADOW_WORLD));
	palette.push_back(Create(B_BLUR));
	palette.push_back(Create(B_MODEL));
}
