// xrCore.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#pragma hdrstop

#include <objbase.h>
#include "xrCore.h"

// #include <string>
 
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

namespace CPU
{
	extern	void			Detect	();
};
void xrCore::_initialize	(LPCSTR _ApplicationName, LogCallback cb)
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
	CPU::Detect				();
	if (strstr(Params,"-mem_debug"))	Memory._initialize		(TRUE);
	else								Memory._initialize		(FALSE);
	InitMath				();
	rtc_initialize			();
	Debug._initialize		();
	xr_FS					= xr_new<CLocatorAPI>	();
	u32 flags				= 0;
	if (0!=strstr(Params,"-build"))	flags |= CLocatorAPI::flBuildCopy;
	if (0!=strstr(Params,"-ebuild"))flags |= CLocatorAPI::flBuildCopy|CLocatorAPI::flEBuildCopy;
	FS._initialize			(flags);
	CreateLog				(cb,0!=strstr(Params,"-nolog"));

	bInitialized			= TRUE;
}

void xrCore::_destroy		()
{
	CloseLog				();
	FS._destroy				();
	xr_delete				(xr_FS);
	Memory._destroy			();
}
