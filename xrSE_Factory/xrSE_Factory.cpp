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

extern CSE_Abstract *F_entity_Create	(LPCSTR section);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

DLL_API CTestInterface *create_test_object	()
{
	return				(xr_new<CTestInterface>());
}

DLL_API void			destroy_test_object	(CTestInterface *&object)
{
	xr_delete			(object);
}

CTestInterface::CTestInterface	()
{
	test0.bind			(this,&CTestInterface::test_0);
	test1.bind			(this,&CTestInterface::test_1);
}

void CTestInterface::test_0		(LPCSTR str)
{
	Msg					("TEST_0 : %s",str);
}

void CTestInterface::test_1		(int num)
{
	Msg					("TEST_1 : %d",num);
}

void CTestInterface::test_test0	()
{
	test0				("VC7.1 : OK!");
}

void CTestInterface::test_test1	()
{
	test1				(1);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

DLL_API CSE_Abstract *create_object	(LPCSTR section)
{
	return				(F_entity_Create(section));
}

DLL_API void destroy_object			(CSE_Abstract *&abstract)
{
	return				(F_entity_Destroy(abstract));
}

BOOL APIENTRY DllMain				(HANDLE module_handle, DWORD call_reason, LPVOID reserved)
{
	switch (call_reason) {
		case DLL_PROCESS_ATTACH: {
 			Core._initialize			("xrSE_Factory",NULL);
			ai().script_engine().export	();
			break;
		}
		case DLL_PROCESS_DETACH: {
			xr_delete					(g_object_factory);
			Core._destroy				();
			break;
		}
	}
    return				(TRUE);
}
