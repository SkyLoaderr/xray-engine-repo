// xrRender_R1.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "xrRender_R1.h"
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

// This is an example of an exported variable
XRRENDER_R1_API int nxrRender_R1=0;

// This is an example of an exported function.
XRRENDER_R1_API int fnxrRender_R1(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see xrRender_R1.h for the class definition
CxrRender_R1::CxrRender_R1()
{ 
	return; 
}
