// xrD3D9-Null.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "xrD3D9-Null.h"
BOOL APIENTRY DllMain( HANDLE hModule, 
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
/*
// This is an example of an exported variable
XRD3D9NULL_API int nxrD3D9Null=0;

// This is an example of an exported function.
XRD3D9NULL_API int fnxrD3D9Null(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see xrD3D9-Null.h for the class definition
CxrD3D9Null::CxrD3D9Null()
{ 
	return; 
}
*/
XRD3D9NULL_API IDirect3D9 *  Direct3DCreate9(UINT SDKVersion)
{
	if (SDKVersion != D3D_SDK_VERSION)
	{
		return NULL;
	}
	return new IDirect3D9();
}

