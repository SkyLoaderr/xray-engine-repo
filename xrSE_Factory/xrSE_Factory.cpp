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
#include "script_properties_list_helper.h"

extern CSE_Abstract *F_entity_Create	(LPCSTR section);

typedef IPropHelper& (__stdcall *TPHelper) ();

TPHelper					_PHelper = 0;
HMODULE						prop_helper_module = 0;
LPCSTR						prop_helper_library = "xrEPropsB.dll", prop_helper_func = "PHelper";
CScriptPropertiesListHelper	*g_property_list_helper = 0;

IPropHelper &PHelper()
{
	R_ASSERT3				(_PHelper,"Cannot find entry point of the function or Cannot find library",prop_helper_library);
	return					(_PHelper());
}

FACTORY_API ISE_Abstract *create_entity	(LPCSTR section)
{
	return					(F_entity_Create(section));
}

FACTORY_API void destroy_entity			(ISE_Abstract *&abstract)
{
	CSE_Abstract			*object = smart_cast<CSE_Abstract*>(abstract);
	F_entity_Destroy		(object);
	abstract				= 0;
}

void load_prop_helper			()
{

	prop_helper_module		= LoadLibrary(prop_helper_library);
	if (!prop_helper_module) {
		Msg					("! Cannot find library %s",prop_helper_library);
		return;
	}
	_PHelper				= (TPHelper)GetProcAddress(prop_helper_module,prop_helper_func);
	if (!_PHelper) {
		Msg					("! Cannot find entry point of the function %s in the library %s",prop_helper_func,prop_helper_func);
		return;
	}
}

BOOL APIENTRY DllMain		(HANDLE module_handle, DWORD call_reason, LPVOID reserved)
{
	switch (call_reason) {
		case DLL_PROCESS_ATTACH: {
 			Core._initialize			("xrSE_Factory",NULL);
			string_path					SYSTEM_LTX;
			FS.update_path				(SYSTEM_LTX,"$game_data$","system.ltx");
			pSettings					= xr_new<CInifile>(SYSTEM_LTX);
			load_prop_helper			();
			if (_PHelper)
				g_property_list_helper	= xr_new<CScriptPropertiesListHelper>();
			ai().script_engine().script_export	();
			break;
		}
		case DLL_PROCESS_DETACH: {
			xr_delete					(g_object_factory);
			xr_delete					(pSettings);
			xr_delete					(g_property_list_helper);
			xr_delete					(g_ai_space);
			xr_delete					(g_object_factory);
			if (prop_helper_module)
				FreeLibrary				(prop_helper_module);
			Core._destroy				();
			break;
		}
	}
    return				(TRUE);
}
