// xrCore.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#pragma hdrstop

#include <objbase.h>
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

	// Init COM so we can use CoCreateInstance
	CoInitializeEx			(NULL, COINIT_MULTITHREADED);

	strcpy					(ApplicationName,_ApplicationName);
	strlwr					(strcpy(Params,GetCommandLine()));

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