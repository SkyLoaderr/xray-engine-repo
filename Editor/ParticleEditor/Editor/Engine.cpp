// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Engine.h"

CEngine	Engine;

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
	InitMath				();
	// Other stuff
	FS.OnCreate				();
}

void CEngine::Destroy()
{                      
	Engine.FS.OnDestroy		();
}
