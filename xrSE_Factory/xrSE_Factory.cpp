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

extern CScriptPropertiesListHelper	*g_property_list_helper;
extern HMODULE						prop_helper_module;

extern "C" {
	FACTORY_API	ISE_Abstract* __stdcall create_entity	(LPCSTR section)
	{
		return					(F_entity_Create(section));
	}

	FACTORY_API	void		__stdcall destroy_entity	(ISE_Abstract *&abstract)
	{
		CSE_Abstract			*object = smart_cast<CSE_Abstract*>(abstract);
		F_entity_Destroy		(object);
		abstract				= 0;
	}
};

BOOL APIENTRY DllMain		(HANDLE module_handle, DWORD call_reason, LPVOID reserved)
{
	switch (call_reason) {
		case DLL_PROCESS_ATTACH: {
 			Core._initialize			("xrSE_Factory",NULL,TRUE,"fsfactory.ltx");
			string_path					SYSTEM_LTX;
			FS.update_path				(SYSTEM_LTX,"$game_config$","system.ltx");
			pSettings					= xr_new<CInifile>(SYSTEM_LTX);
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
