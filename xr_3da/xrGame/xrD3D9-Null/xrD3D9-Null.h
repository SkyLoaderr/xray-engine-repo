// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XRD3D9NULL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XRD3D9NULL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef XRD3D9NULL_EXPORTS
#define XRD3D9NULL_API __declspec(dllexport)
#else
#define XRD3D9NULL_API __declspec(dllimport)
#endif
//---------------------------------
#include <stdlib.h>
#include <objbase.h>
#include <windows.h>
//---------------------------------
#include "d3d9types.h"
#include "d3d9caps.h"
//---------------------------------
#ifndef DIRECT3D_VERSION
#define DIRECT3D_VERSION         0x0900
#endif  //DIRECT3D_VERSION

#define D3D_SDK_VERSION   (32 | 0x80000000)


/*
// This class is exported from the xrD3D9-Null.dll
class XRD3D9NULL_API CxrD3D9Null {
public:
	CxrD3D9Null(void);
	// TODO: add your methods here.
};

extern XRD3D9NULL_API int nxrD3D9Null;

XRD3D9NULL_API int fnxrD3D9Null(void);
*/

#include "IDirect3D9.h"



extern "C" XRD3D9NULL_API IDirect3D9* FAR PASCAL  Direct3DCreate9(UINT SDKVersion);



