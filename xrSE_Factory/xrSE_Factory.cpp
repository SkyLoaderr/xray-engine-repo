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

extern CSE_Abstract *F_entity_Create	(LPCSTR section);

DLL_API CSE_Abstract *create_object	(LPCSTR section)
{
	return				(F_entity_Create(section));
}

DLL_API void destroy_object			(CSE_Abstract *&abstract)
{
	return				(F_entity_Destroy(abstract));
}

BOOL APIENTRY DllMain				(HANDLE module_handle, DWORD call_reason, LPVOID reserved)
{
	switch (call_reason) {
		case DLL_PROCESS_ATTACH: {
 			Core._initialize			("xrSE_Factory",NULL);
			ai().script_engine().export	();
			break;
		}
		case DLL_PROCESS_DETACH: {
			Core._destroy				();
			break;
		}
	}
    return				(TRUE);
}
