// Blender.cpp: implementation of the CBlender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Blender.h"

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
	FS.write	(&description,sizeof(description));
	xrPWRITE_MARKER ("General");
	xrPWRITE_PROP	("Priority",		xrPID_INTEGER,	oPriority);
	xrPWRITE_PROP	("Strict sorting",	xrPID_BOOL,		oStrictSorting);
	xrPWRITE_MARKER	("Base Texture");
	xrPWRITE_PROP	("Name",			xrPID_TEXTURE,	oT_Name);
	xrPWRITE_PROP	("Transform",		xrPID_MATRIX,	oT_xform);
}

void	CBlender::Load(	CStream& FS )
{
	FS.Read		(&description,sizeof(description));
	xrPREAD_MARKER	();
	xrPREAD_PROP		(xrPID_INTEGER,	oPriority);
	xrPREAD_PROP		(xrPID_BOOL,	oStrictSorting);
	xrPREAD_MARKER	();
	xrPREAD_PROP		(xrPID_TEXTURE,	oT_Name);
	xrPREAD_PROP		(xrPID_MATRIX,	oT_xform);
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
}
