////////////////////////////////////////////////////////////////////////////
//	Module 		: xrGame.cpp
//	Created 	: 07.01.2001
//  Modified 	: 27.05.2004
//	Author		: Aleksandr Maksimchuk and Oles' Shyshkovtsov
//	Description : Defines the entry point for the DLL application.
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_factory.h"
#include "ui/xrXMLParser.h"
#include "xr_level_controller.h"
#include "profiler.h"
#include "ai_space.h"
#include "script_engine.h"

#pragma comment(lib,"ode.lib" )

extern "C" {
	DLL_API DLL_Pure*	__cdecl xrFactory_Create		(CLASS_ID clsid)
	{
		DLL_Pure			*object = object_factory().client_object(clsid);
		object->CLS_ID		= clsid;
		return				(object);
	}

	DLL_API void		__cdecl	xrFactory_Destroy		(DLL_Pure* O)
	{
		xr_delete			(O);
	}
};

extern void CCC_RegisterCommands();

BOOL APIENTRY DllMain(HANDLE hModule, u32 ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH: {
			ai().script_engine().script_export();
			// register console commands
			CCC_RegisterCommands();
			// keyboard binding
			CCC_RegisterInput			();
			// xml parser options
			XML_DisableStringCaching	();
			// script registration
#ifdef DEBUG
			g_profiler			= xr_new<CProfiler>();
#endif
			break;
		}

		case DLL_PROCESS_DETACH: {
			break;
		}
	}
    return								(TRUE);
}
