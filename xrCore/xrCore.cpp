// xrCore.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#pragma hdrstop

#include "xrCore.h"

#include <string>
 
#pragma comment(lib,"winmm.lib")

XRCORE_API		xrCore Core;

BOOL APIENTRY	DllMain(	HANDLE hModule, 
							DWORD  ul_reason_for_call, 
							LPVOID lpReserved
						)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

void xrCore::_initialize	(LPCSTR _ApplicationName, LogCallback cb, LPCSTR root_path)
{
	static BOOL				bInitialized	= FALSE;

	if (bInitialized)		return;

	strcpy					(ApplicationName,_ApplicationName);
//.	R_ASSERT2				(!bInitialized, "xrCore already initialized.");

	// Parameters
	strcpy					(Params,GetCommandLine());
	strlwr					(Params);

	// User/Comp Name
	DWORD	sz_user			= sizeof(UserName);
	GetUserName				(UserName,&sz_user);

	DWORD	sz_comp			= sizeof(CompName);
	GetComputerName			(CompName,&sz_comp);

	// Mathematics & PSI detection
	InitMath				();
	rtc_initialize			();
	Debug._initialize		();
	Memory._initialize		();
	xr_FS					= xr_new<CLocatorAPI>	();
	FS._initialize			(0!=strstr(Params,"-build"),root_path);
	CreateLog				(cb);

	bInitialized			= TRUE;
}

void xrCore::_destroy		()
{
	FS._destroy				();
	CloseLog				();
}