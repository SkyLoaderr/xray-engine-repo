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

DLL_API CSE_Abstract *create_object(const CLASS_ID &clsid, LPCSTR section);

#endif // XRSE_FACTORY_H