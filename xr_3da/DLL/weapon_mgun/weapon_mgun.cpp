// weapon_mgun.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "WeaponMGun.h"
#include "..\\..\\std_classes.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern "C" DLL_API VOID __cdecl _dll_register(CLASS_ID &CLS_ID, CLASS_ID &SUB_CLS_ID)
{
	CLS_ID		=CLSID_OBJECT;
	SUB_CLS_ID	=CLSID_OBJECT_W_MGUN;
}

extern "C" DLL_API DLL_Pure* __cdecl _dll_create(LPVOID params)
{
	return new CWeaponMGun(params);
}

#undef delete
extern "C" DLL_API VOID __cdecl _dll_destroy(DLL_Pure* p)
{
	delete ((CWeaponMGun *)p);
}
