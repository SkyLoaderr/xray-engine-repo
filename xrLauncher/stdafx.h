// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once

#define ENGINE_API		__declspec(dllimport)
#define XRCORE_API		__declspec(dllimport)

typedef char*				LPSTR;
typedef const char*			LPCSTR;
typedef int					BOOL;

typedef char string16 [16];
typedef char string32 [32];
typedef char string64 [64];
typedef char string128 [128];
typedef char string256 [256];
typedef char string512 [512];
typedef char string1024 [1024];
typedef char string2048 [2048];
typedef char string4096 [4096];
