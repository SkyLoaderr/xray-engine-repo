////////////////////////////////////////////////////////////////////////////
//	Module 		: xrSE_Factory.cpp
//	Created 	: 18.06.2004
//  Modified 	: 18.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Precompiled header creatore
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrSE_Factory.h"
#include "ai_space.h"
#include "script_engine.h"

DLL_API CSE_Abstract *create_object(LPCSTR section)
{
	return				(F_entity_Create(section));
}

BOOL APIENTRY DllMain	(HANDLE module_handle, DWORD call_reason, LPVOID reserved)
{
	switch (call_reason) {
		case DLL_PROCESS_ATTACH: {
			ai().script_engine().export	();
			break;
		}
		case DLL_PROCESS_DETACH: {
			break;
		}
	}
    return				(TRUE);
}
