// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// C RunTime Header Files
//#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
//#include <tchar.h>


#include "../xray/xr3DA/stdafx.h"
#include "../xray/xr3DA/xr_ioconsole.h"
#include "xrLauncher.h"

void convert(System::String* src, char* dst);
void setCoreParam(LPCSTR str);
void resetCoreParam(LPCSTR str);
bool testCoreParam(LPCSTR str);