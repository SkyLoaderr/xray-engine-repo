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
	DLL_API CSE_Abstract	*create_object	(LPCSTR section);
	DLL_API void			destroy_object	(CSE_Abstract *&abstract);
//	DLL_API CTestInterface	*create_test_object	();
//	DLL_API void			destroy_test_object	(CSE_Abstract *&CTestInterface);
};

#endif // XRSE_FACTORY_H