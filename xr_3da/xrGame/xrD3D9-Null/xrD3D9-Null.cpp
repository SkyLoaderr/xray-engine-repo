// xrD3D9-Null.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "stdio.h"
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

 IDirect3D9 * WINAPI Direct3DCreate9(UINT SDKVersion)
{
	if (SDKVersion != D3D_SDK_VERSION)
	{
		return NULL;
	}
	xrIDirect3D9* I = new xrIDirect3D9();
	return I;
}

//-----------------------------------------------------------------------
void LogOut( const char *format, ... ) 
{
	va_list argptr;
	char text[4096];
		
	va_start (argptr,format);
	vsprintf (text, format, argptr);
	va_end (argptr);
	
	//rr  printf(text);
	OutputDebugString( text );
}