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

void CBlender::BP_write_c(CFS_Base& FS, DWORD ID, LPCSTR name, LPCVOID data, DWORD size )
{
	FS.Wdword	(ID);
	FS.WstringZ	(name);
	if (data && size)	FS.write	(data,size);
}

DWORD CBlender::BP_read_c(CStream& FS)
{
	char		temp[256];

	DWORD T		= FS.Rdword();
	FS.RstringZ	(temp);
	return		T;
}

void	CBlender::Save(	CFS_Base& FS )
{
	FS.write	(&description,sizeof(description));
	BP_W_MARKER ("General");
	BP_WRITE	("Priority",		BPID_INTEGER,	oPriority);
	BP_WRITE	("Strict sorting",	BPID_BOOL,		oStrictSorting);
	BP_W_MARKER	("Base Texture");
	BP_WRITE	("Name",			BPID_TEXTURE,	oT_Name);
	BP_WRITE	("Transform",		BPID_MATRIX,	oT_xform);
}

void	CBlender::Load(	CStream& FS )
{
	FS.Read		(&description,sizeof(description));
	BP_R_MARKER	();
	BP_READ		(BPID_INTEGER,	oPriority);
	BP_READ		(BPID_BOOL,		oStrictSorting);
	BP_R_MARKER	();
	BP_READ		(BPID_TEXTURE,	oT_Name);
	BP_READ		(BPID_MATRIX,	oT_xform);
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
	palette.push_back(Create(B_SCREEN_BLEND));
	palette.push_back(Create(B_EDITOR_WIRE));
	palette.push_back(Create(B_EDITOR_SEL));
}
