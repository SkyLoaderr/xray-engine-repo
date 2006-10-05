// xrCore.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#pragma hdrstop

#include <objbase.h>
#include "xrCore.h"
 
#pragma comment(lib,"winmm.lib")

XRCORE_API		xrCore Core;

#ifndef XRCORE_STATIC

//. why ??? 
#ifdef _EDITOR
	BOOL WINAPI DllEntryPoint(HINSTANCE hinstDLL, DWORD ul_reason_for_call, LPVOID lpvReserved)
#else
	BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD ul_reason_for_call, LPVOID lpvReserved)
#endif
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			_clear87		();
			_control87		( _PC_53,   MCW_PC );
			_control87		( _RC_CHOP, MCW_RC );
			_control87		( _RC_NEAR, MCW_RC );
			_control87		( _MCW_EM,  MCW_EM );
		}
		break;
	case DLL_THREAD_ATTACH:
		CoInitializeEx	(NULL, COINIT_MULTITHREADED);
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
#endif

namespace CPU
{
	extern	void			Detect	();
};
static u32	init_counter	= 0;

void xrCore::_initialize	(LPCSTR _ApplicationName, LogCallback cb, BOOL init_fs, LPCSTR fs_fname)
{
	strcpy					(ApplicationName,_ApplicationName);
	if (0==init_counter){	
#ifdef XRCORE_STATIC	
		_clear87	();
		_control87	( _PC_53,   MCW_PC );
		_control87	( _RC_CHOP, MCW_RC );
		_control87	( _RC_NEAR, MCW_RC );
		_control87	( _MCW_EM,  MCW_EM );
#endif
		// Init COM so we can use CoCreateInstance
		CoInitializeEx		(NULL, COINIT_MULTITHREADED);

		strlwr				(strcpy(Params,GetCommandLine()));

		string_path		fn,dr,di;

		// application path
        GetModuleFileName(GetModuleHandle(MODULE_NAME),fn,sizeof(fn));
        _splitpath		(fn,dr,di,0,0);
        strconcat		(ApplicationPath,dr,di);                                       

		// application data path
		R_CHK			(GetEnvironmentVariable("USERPROFILE",fn,sizeof(fn)));
		u32 fn_len		= xr_strlen(fn);
		if (fn_len && fn[fn_len-1]=='\\') fn[fn_len-1]=0;

		// working path
		GetCurrentDirectory(sizeof(WorkingPath),WorkingPath);

//.		strconcat		(ApplicationDataPath,fn,"\\Application Data\\",COMPANY_NAME,"\\",PRODUCT_NAME);
//.		strcpy			(ApplicationDataPath, ApplicationPath);
		strcpy			(ApplicationDataPath, WorkingPath);

//		_splitpath		(fn,dr,di,0,0);
//		strconcat		(ApplicationDataPath,dr,di);                                       


		// User/Comp Name
		DWORD	sz_user		= sizeof(UserName);
		GetUserName			(UserName,&sz_user);

		DWORD	sz_comp		= sizeof(CompName);
		GetComputerName		(CompName,&sz_comp);

		// Mathematics & PSI detection
		CPU::Detect			();
		if (strstr(Params,"-mem_debug"))	Memory._initialize		(TRUE);
		else								Memory._initialize		(FALSE);
		_initialize_cpu		();
		Debug._initialize	();

		rtc_initialize		();
		xr_FS				= xr_new<CLocatorAPI>	();
		xr_EFS				= xr_new<EFS_Utils>		();
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
		FS._initialize		(flags,0,fs_fname);
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
