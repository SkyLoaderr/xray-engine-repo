// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Engine.h"

xrDispatchTable	PSGP;
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

LPCSTR CEngine::LastWindowsError()
{
	static string1024 errmsg_buf;
    LPCSTR err=0;

    DWORD hr=GetLastError();
	if (hr!=0) {
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,hr,0,errmsg_buf,1024,0);
        err = errmsg_buf;
	}
    return err;
}

void CEngine::Initialize(void)
{
	// Other stuff
	FS.OnCreate				();

	string256               fn;
    strconcat               (fn,_EDITOR_FILE_NAME_,".log");
    FS.m_LocalRoot.Update   (fn);
    ELog.Create             (fn);

	// Mathematics & PSI detection
	InitMath				();

	// Bind PSGP
	hPSGP		            = LoadLibrary("xrCPU_Pipe.dll");
	R_ASSERT	            (hPSGP);

	xrBinder*	bindCPU	    = (xrBinder*)	GetProcAddress(hPSGP,"xrBind_PSGP");	R_ASSERT(bindCPU);
	bindCPU		            (&PSGP, CPU::ID.feature & CPU::ID.os_support);
}

void CEngine::Destroy()
{                      
	Engine.FS.OnDestroy		();
}
