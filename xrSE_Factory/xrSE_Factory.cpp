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
#include "object_factory.h"
#include "xrEProps.h"
#include "xrSE_Factory_import_export.h"

extern CSE_Abstract *F_entity_Create	(LPCSTR section);

typedef IPropHelper& (__stdcall *TPHelper) ();

TPHelper	_PHelper = 0;
HMODULE		prop_helper_module = 0;
LPCSTR		prop_helper_library = "xrEPropsB.dll", prop_helper_func = "PHelper";

IPropHelper &PHelper()
{
	R_ASSERT3					(_PHelper,"Cannot find entry point of the function or Cannot find library",prop_helper_library);
	return						(_PHelper());
}

DLL_API CSE_Abstract *create_entity	(LPCSTR section)
{
	return				(F_entity_Create(section));
}

DLL_API void destroy_entity			(CSE_Abstract *&abstract)
{
	return				(F_entity_Destroy(abstract));
}

void load_prop_helper			()
{

	prop_helper_module			= LoadLibrary(prop_helper_library);
	if (!prop_helper_module) {
		Msg						("! Cannot find library %s",prop_helper_library);
		return;
	}
	_PHelper					= (TPHelper)GetProcAddress(prop_helper_module,prop_helper_func);
	if (!_PHelper) {
		Msg						("! Cannot find entry point of the function %s in the library %s",prop_helper_func,prop_helper_func);
		return;
	}
}

BOOL APIENTRY DllMain				(HANDLE module_handle, DWORD call_reason, LPVOID reserved)
{
	switch (call_reason) {
		case DLL_PROCESS_ATTACH: {
 			Core._initialize			("xrSE_Factory",NULL);
			pSettings					= xr_new<CInifile>(SYSTEM_LTX);
			load_prop_helper			();
			ai().script_engine().export	();
			break;
		}
		case DLL_PROCESS_DETACH: {
			xr_delete					(g_object_factory);
			if (prop_helper_module)
				FreeLibrary				(prop_helper_module);
			Core._destroy				();
			break;
		}
	}
    return				(TRUE);
}
