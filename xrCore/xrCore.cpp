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

void xrCore::_initialize	(LPCSTR _ApplicationName, LogCallback cb)
{
	static BOOL				bInitialized	= FALSE;

	strcpy					(ApplicationName,_ApplicationName);
	R_ASSERT2				(!bInitialized, "xrCore already initialized.");

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

	Debug._initialize		();
	Memory._initialize		();
	FS._initialize			();

	CreateLog				(cb);

	bInitialized			= TRUE;
}

void xrCore::_destroy		()
{
	FS._destroy				();
	CloseLog				();
}