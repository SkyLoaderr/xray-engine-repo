// Blender.cpp: implementation of the CBlender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Blender.h"

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

void	CBlender::Save(	CFS_Base& FS )
{
	BP_W_MARKER ("General");
	BP_WRITE	("Priority",		BPID_INTEGER,	oPriority);
	BP_WRITE	("Strict sorting",	BPID_BOOL,		oStrictSorting);
	BP_W_MARKER	("Base Texture");
	BP_WRITE	("TC source",		BPID_TCS,		oTCS);
	BP_WRITE	("TC modifier",		BPID_TCM,		oTCM);
}

void	CBlender::Load(	CStream& FS )
{
	BP_R_MARKER	();
	BP_READ		(BPID_INTEGER,	oPriority);
	BP_READ		(BPID_BOOL,		oStrictSorting);
	BP_R_MARKER	();
	BP_READ		(BPID_TCS,		oTCS);
	BP_READ 	(BPID_TCM,		oTCM);
}
