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

}

CBlender::~CBlender()
{

}

void CBlender::BP_write(CFS_Base& FS, DWORD ID, LPCSTR name, LPCVOID data, DWORD size )
{
	FS.Wdword	(ID);
	FS.WstringZ	(name);
	FS.write	(data,size);
}

DWORD CBlender::BP_read(CStream& FS)
{
	char		temp[256];

	DWORD T		= FS.Rdword();
	FS.RstringZ	(temp);
	return		T;
}
