// xrCore.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#pragma hdrstop

#include <objbase.h>
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

namespace CPU
{
	extern	void			Detect	();
};
static u32	init_counter	= 0;
void xrCore::_initialize	(LPCSTR _ApplicationName, LogCallback cb, BOOL init_fs)
{
	strcpy					(ApplicationName,_ApplicationName);
	if (0==init_counter){	
		// Init COM so we can use CoCreateInstance
		CoInitializeEx		(NULL, COINIT_MULTITHREADED);

		strlwr				(strcpy(Params,GetCommandLine()));

        string_path		fn,dr,di;
        GetModuleFileName(GetModuleHandle(MODULE_NAME),fn,sizeof(fn));
        _splitpath		(fn,dr,di,0,0);
        strconcat		(ApplicationPath,dr,di);                                       

		// User/Comp Name
		DWORD	sz_user		= sizeof(UserName);
		GetUserName			(UserName,&sz_user);

		DWORD	sz_comp		= sizeof(CompName);
		GetComputerName		(CompName,&sz_comp);

		// Mathematics & PSI detection
		CPU::Detect			();
		if (strstr(Params,"-mem_debug"))	Memory._initialize		(TRUE);
		else								Memory._initialize		(FALSE);
Memory.dbg_check();
		InitMath			();
Memory.dbg_check();
		Debug._initialize	();
Memory.dbg_check();

		rtc_initialize		();
Memory.dbg_check();
		xr_FS				= xr_new<CLocatorAPI>	();
Memory.dbg_check();
		xr_EFS				= xr_new<EFS_Utils>		();
Memory.dbg_check();
	}
	if (init_fs){
		u32 flags			= 0;
		if (0!=strstr(Params,"-build"))	 flags |= CLocatorAPI::flBuildCopy;
		if (0!=strstr(Params,"-ebuild")) flags |= CLocatorAPI::flBuildCopy|CLocatorAPI::flEBuildCopy;
#ifdef	DEBUG
		if (0==strstr(Params,"-nocache"))flags |= CLocatorAPI::flCacheFiles;
#endif
#ifdef	_EDITOR // for EDITORS - no cache
		flags 				&=~ CLocatorAPI::flCacheFiles;
#endif
		FS._initialize		(flags);
		EFS._initialize		();
	}
	SetLogCB				(cb);
	init_counter++;
}

void xrCore::_destroy		()
{
	--init_counter;
	if (0==init_counter){
		FS._destroy			();
		EFS._destroy		();
		xr_delete			(xr_FS);
		xr_delete			(xr_EFS);
		Memory._destroy		();
	}
}
