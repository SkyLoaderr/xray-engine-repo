// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Engine.h"

CEngine				Engine;
xrDispatchTable		PSGP;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngine::CEngine()
{
	
}

CEngine::~CEngine()
{
	
}

void CEngine::Initialize	(void)
{
	strcpy								(Params,GetCommandLine());
	strlwr								(Params);

	// Mathematics & PSI detection
	InitMath							( );
	
	// Bind PSGP
	hPSGP		= LoadLibrary("xrCPU_Pipe.dll");
	R_ASSERT	(hPSGP);
	
	xrBinder*	bindCPU	= (xrBinder*)	GetProcAddress(hPSGP,"xrBind_PSGP");	R_ASSERT(bindCPU);
	bindCPU		(&PSGP, CPU::ID.feature & CPU::ID.os_support);
	
	// Other stuff
	Engine.FS.Initialize				( );
	Engine.Scripts.Initialize			( );
	Engine.Sheduler.Initialize			( );
}

void CEngine::Destroy	()
{
	Engine.Sheduler.Destroy				( );
	Engine.External.Destroy				( );
	Engine.Scripts.Destroy				( );
	Engine.FS.Destroy					( );
	
	if (hPSGP)	{ FreeLibrary(hPSGP); hPSGP=0; }
}

void CEngine::mem_Compact()
{
	_heapmin			();
	HeapCompact			(GetProcessHeap(),0);
}

DWORD CEngine::mem_Usage()
{
	_HEAPINFO		hinfo;
	int				heapstatus;
	hinfo._pentry	= NULL;
	DWORD	total	= 0;
	while( ( heapstatus = _heapwalk( &hinfo ) ) == _HEAPOK )
	{ 
		if (hinfo._useflag == _USEDENTRY)	total += hinfo._size;
	}
	
	switch( heapstatus )
	{
	case _HEAPEMPTY:
		break;
	case _HEAPEND:
		break;
	case _HEAPBADPTR:
		Msg( "! ERROR - bad pointer to heap" );
		break;
	case _HEAPBADBEGIN:
		Msg( "! ERROR - bad start of heap" );
		break;
	case _HEAPBADNODE:
		Msg( "! ERROR - bad node in heap" );
		break;
	}
	return total;
}
