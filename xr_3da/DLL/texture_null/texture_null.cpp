// texture_null.cpp : Defines the entry point for the DLL application.
// 
// Made by Oles Shishkovtsov for "XRay Engine"
// Kiev, 2000

#include "stdafx.h"
#include "TextureNull.h"
#include "..\..\std_classes.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern "C" DLL_API VOID __cdecl _dll_register(CLASS_ID &CLS_ID, CLASS_ID &SUB_CLS_ID)
{
	CLS_ID		=CLSID_TEXTURE;
	SUB_CLS_ID	=CLSID_TEXTURE_NULL;
}

extern "C" DLL_API DLL_Pure* __cdecl _dll_create(LPVOID params)
{
	return new CTextureNull(params);
}

#undef delete
extern "C" DLL_API VOID __cdecl _dll_destroy(DLL_Pure* p)
{
	delete ((CTextureNull *)p);
}
