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

void	CBlender::Save(IWriter& FS )
{
	FS.w			(&description,sizeof(description));
	xrPWRITE_MARKER (FS,"General");
	xrPWRITE_PROP	(FS,"Priority",			xrPID_INTEGER,	oPriority);
	xrPWRITE_PROP	(FS,"Strict sorting",	xrPID_BOOL,		oStrictSorting);
	xrPWRITE_MARKER	(FS,"Base Texture");
	xrPWRITE_PROP	(FS,"Name",				xrPID_TEXTURE,	oT_Name);
	xrPWRITE_PROP	(FS,"Transform",		xrPID_MATRIX,	oT_xform);
}

void	CBlender::Load(	IReader& FS, WORD version )
{
	// Read desc and doesn't change version
	WORD	V		= description.version;
	FS.r			(&description,sizeof(description));
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
