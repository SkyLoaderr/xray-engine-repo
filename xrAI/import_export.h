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

class DLL_API CTestInterface {
public:
	FastDelegate1<LPCSTR>	test0;
	FastDelegate1<int>		test1;

private:
			void			test_0			(LPCSTR);
			void			test_1			(int);

public:
							CTestInterface	();
	virtual void			test_test0		();
	virtual void			test_test1		();
};

#endif // IMPORT_EXPORT_H