////////////////////////////////////////////////////////////////////////////
//	Module 		: xrSE_Factory.h
//	Created 	: 18.06.2004
//  Modified 	: 18.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Precompiled header creatore
////////////////////////////////////////////////////////////////////////////

#ifndef XRSE_FACTORY_H
#	define XRSE_FACTORY_H

#include "xrServer_Objects.h"
#include "import_export.h"

extern "C" {
	DLL_API CSE_Abstract *	__cdecl create_object(const CLASS_ID &clsid, LPCSTR section);
	DLL_API LPCSTR			__cdecl get_string();
	DLL_API CTestInterface*	__cdecl get_object(int index);
};

#endif // XRSE_FACTORY_H