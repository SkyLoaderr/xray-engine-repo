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
	const DWORD comp = MAX_COMPUTERNAME_LENGTH + 1;
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

void	CBlender::Load(	CStream& FS )
{
	FS.Read			(&description,sizeof(description));
	xrPREAD_MARKER	(FS);
	xrPREAD_PROP	(FS,xrPID_INTEGER,	oPriority);
	xrPREAD_PROP	(FS,xrPID_BOOL,		oStrictSorting);
	xrPREAD_MARKER	(FS);
	xrPREAD_PROP	(FS,xrPID_TEXTURE,	oT_Name);
	xrPREAD_PROP	(FS,xrPID_MATRIX,	oT_xform);
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
