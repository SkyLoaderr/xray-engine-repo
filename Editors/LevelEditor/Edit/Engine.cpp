// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Engine.h"
#include "xr_ini.h"

xrDispatchTable	PSGP;
CEngine	Engine;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngine::CEngine()
{
	hPSGP = 0;
	ZeroMemory(&PSGP,sizeof(PSGP));
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

extern void __stdcall xrSkin1W_x86	(vertRender* D, vertBoned1W* S, u32 vCount, CBoneInstance* Bones);
extern void __stdcall xrSkin2W_x86	(vertRender* D, vertBoned2W* S, u32 vCount, CBoneInstance* Bones);
extern void __stdcall xrBoneLerp_x86(CKey* D, CKeyQ* K1, CKeyQ* K2, float delta);

void CEngine::Initialize(void)
{
	strcpy					(Params,GetCommandLine());
    strlwr					(Params);

	// Other stuff
	FS.OnCreate				();

	string256               fn;
    strconcat               (fn,_EDITOR_FILE_NAME_,".log");
    FS.m_LocalRoot.Update   (fn);
    ELog.Create             (fn);

	// Mathematics & PSI detection
	InitMath				();

#ifdef _EDITOR
	// Bind PSGP
	hPSGP		            = LoadLibrary("xrCPU_Pipe.dll");
	R_ASSERT2	            (hPSGP,"Can't find 'xrCPU_Pipe.dll'");

	xrBinder*	bindCPU	    = (xrBinder*)	GetProcAddress(hPSGP,"xrBind_PSGP");	R_ASSERT(bindCPU);
	bindCPU		            (&PSGP, CPU::ID.feature & CPU::ID.os_support);
    // for compliance with editor
    PSGP.skin1W				= xrSkin1W_x86;
    PSGP.skin2W				= xrSkin2W_x86;
#endif

    // game configure
    string256 si_name		= "system.ltx";
    FS.m_GameRoot.Update	(si_name);
	pSettings				= new CInifile(si_name,TRUE);// FALSE,TRUE,TRUE);

}

void CEngine::Destroy()
{                      
    _DELETE					(pSettings);
	Engine.FS.OnDestroy		();
	if (hPSGP)	{ FreeLibrary(hPSGP); hPSGP=0; }
}
