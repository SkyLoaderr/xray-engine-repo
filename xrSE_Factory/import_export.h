////////////////////////////////////////////////////////////////////////////
//	Module 		: import_export.h
//	Created 	: 18.06.2004
//  Modified 	: 18.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Precompiled header creatore
////////////////////////////////////////////////////////////////////////////

#ifndef IMPORT_EXPORT_H
#	define IMPORT_EXPORT_H

#include "fastdelegate.h"

using namespace fastdelegate;

struct CTestInterface {
	virtual void							get_string(LPCSTR&) = 0;
			FastDelegate1<LPCSTR&>			OnGetString;
};

#endif // IMPORT_EXPORT_H

