// xrCore.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "xrCore.h"

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

void xrCore::_initialize	(const char* _ApplicationName)
{
	// Parameters
	strcpy					(Params,GetCommandLine());
	strlwr					(Params);
	strcpy					(ApplicationName,_ApplicationName);

	// User Name
	DWORD	sz_user				= sizeof(UserName);
	GetUserName					(UserName,&sz_user);

	// Mathematics & PSI detection
	InitMath				();

	Debug._initialize		();
	Memory._initialize		();
}
