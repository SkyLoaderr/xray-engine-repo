// Blender.cpp: implementation of the CBlender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Blender.h"

BP_TCS	CBlender::oTCS_identity;
BP_TCM	CBlender::oTCM_identity;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender::CBlender()
{
	oPriority.min	= 0;
	oPriority.max	= 3;
	oPriority.value	= 1;
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

BOOL	CBlender::c_XForm()
{
	// Detect if XForm is needed at all
	if (oTCS.tcs != BP_TCS::tcsGeometry)	return TRUE;
	if (oTCM.tcm != 0)						return TRUE;
	return FALSE;
}

void	CBlender::Save(	CFS_Base& FS )
{
	FS.write	(&description,sizeof(description));
	BP_W_MARKER ("General");
	BP_WRITE	("Priority",		BPID_INTEGER,	oPriority);
	BP_WRITE	("Strict sorting",	BPID_BOOL,		oStrictSorting);
	BP_W_MARKER	("Base Texture");
	BP_WRITE	("TC source",		BPID_TCS,		oTCS);
	BP_WRITE	("TC modifier",		BPID_TCM,		oTCM);
}

void	CBlender::Load(	CStream& FS )
{
	FS.Read		(&description,sizeof(description));
	BP_R_MARKER	();
	BP_READ		(BPID_INTEGER,	oPriority);
	BP_READ		(BPID_BOOL,		oStrictSorting);
	BP_R_MARKER	();
	BP_READ		(BPID_TCS,		oTCS);
	BP_READ 	(BPID_TCM,		oTCM);
}
//////////////////////////////////////////////////////////////////////
#include "blender_clsid.h"
#include "blenderdefault.h"
#include "blender_default_aref.h"
#include "blender_vertex.h"
#include "blender_aref.h"
CBlender*	CBlender::Create	(CLSID cls)
{	
	switch (cls)
	{
	case B_DEFAULT:			return new CBlender_default;		break;
	case B_DEFAULT_AREF:	return new CBlender_default_aref;	break;
	case B_VERT:			return new CBlender_Vertex;			break;
	case B_VERT_AREF:		return new CBlender_Vertex_aref;	break;
	default:				return 0;
	}
}
