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

void	CBlender::Save(IWriter& fs )
{
	fs.w			(&description,sizeof(description));
	xrPWRITE_MARKER (fs,"General");
	xrPWRITE_PROP	(fs,"Priority",			xrPID_INTEGER,	oPriority);
	xrPWRITE_PROP	(fs,"Strict sorting",	xrPID_BOOL,		oStrictSorting);
	xrPWRITE_MARKER	(fs,"Base Texture");
	xrPWRITE_PROP	(fs,"Name",				xrPID_TEXTURE,	oT_Name);
	xrPWRITE_PROP	(fs,"Transform",		xrPID_MATRIX,	oT_xform);
}

void	CBlender::Load(	IReader& fs, WORD version )
{
	// Read desc and doesn't change version
	WORD	V		= description.version;
	fs.r			(&description,sizeof(description));
	description.version	= V;

	// Properties
	xrPREAD_MARKER	(fs);
	xrPREAD_PROP	(fs,xrPID_INTEGER,	oPriority);
	xrPREAD_PROP	(fs,xrPID_BOOL,		oStrictSorting);
	xrPREAD_MARKER	(fs);
	xrPREAD_PROP	(fs,xrPID_TEXTURE,	oT_Name);
	xrPREAD_PROP	(fs,xrPID_MATRIX,	oT_xform);
}

void	CBlender::Compile(CBlender_Compile& C)
{
	if (C.bEditor)	C.SetParams	(oPriority.value,oStrictSorting.value?true:false,true,	false);
	else			C.SetParams	(oPriority.value,oStrictSorting.value?true:false,false,	false);
}
