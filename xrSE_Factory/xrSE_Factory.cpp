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

#include <boost/function.hpp>

extern CSE_Abstract *F_entity_Create	(LPCSTR section);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class DLL_API CTestImpl:public CTestInterface{
private:
		void			test_0			(LPCSTR);
		void			test_1			(int);
public:
						CTestImpl		();
	virtual void		test_test0		();
	virtual void		test_test1		();
};

DLL_API CTestInterface *create_test_object	()
{
	return				(xr_new<CTestImpl>());
}

DLL_API void			destroy_test_object	(CTestInterface *&object)
{
	xr_delete			(object);
}

CTestImpl::CTestImpl	()
{
	test0.bind			(this,&CTestImpl::test_0);
	test1.bind			(this,&CTestImpl::test_1);
}

void CTestImpl::test_0		(LPCSTR str)
{
	Msg					("TEST_0 : %s",str);
}

void CTestImpl::test_1		(int num)
{
	Msg					("TEST_1 : %d",num);
}

void CTestImpl::test_test0	()
{
//	CTestImpl*			self;
//	__asm mov [self],eax;
//	self->test0			("VC7.1 : OK!");
	test0				("VC7.1 : OK!");
}

void CTestImpl::test_test1	()
{
//	CTestImpl*			self;
//	__asm mov [self],eax;
//	self->test_1		(1);
	test_1				(1);
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
