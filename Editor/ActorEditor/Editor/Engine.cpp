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

void CEngine::Initialize(void)
{
	// Mathematics & PSI detection
	InitMath							( );

	// Bind PSGP
	Log("Loading PSGP: xrCPU_Pipe.dll");
	hPSGP		= LoadLibrary("xrCPU_Pipe.dll");
	R_ASSERT	(hPSGP);

	xrBinder*	bindCPU	= (xrBinder*)	GetProcAddress(hPSGP,"xrBind_PSGP");	R_ASSERT(bindCPU);
	bindCPU		(&PSGP);

	// Other stuff
	Engine.FS.Initialize				( );
	Engine.Scripts.Initialize			( );
}

void CEngine::Destroy	()
{
	Engine.External.Destroy				( );
	Engine.Scripts.Destroy				( );
	Engine.FS.Destroy					( );

	if (hPSGP)	{ FreeLibrary(hPSGP); hPSGP=0; }
}
