////////////////////////////////////////////////////////////////////////////
//	Module 		: import_export.h
//	Created 	: 18.06.2004
//  Modified 	: 18.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Precompiled header creatore
////////////////////////////////////////////////////////////////////////////

#ifndef IMPORT_EXPORT_H
#	define IMPORT_EXPORT_H

struct CTestInterface {
	virtual LPCSTR get_string() = 0;
};

#endif

