#include "stdafx.h"	

#pragma warning(push)
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#include "xrCore.h"
#include <mmsystem.h>
#pragma warning(pop)

#include "time_smart_ptr.h"	
#include "time_intrusive_ptr.h"	
#include "object_broker.h"	

#pragma comment(lib,"Winmm.lib")

struct CTest : public CTimeIntrusiveBase {
	virtual ~CTest()
	{
		printf			("CTest::~CTest();");
	}
};

template <typename T>
struct fwd {
	template <bool a>
	IC	static void foo();
	template <>
	IC	static void foo<true>();
};

template <typename T>
template <bool a>
IC	void fwd<T>::foo<>()
{
	printf("false\n");
}

template <typename T>
template <>
IC	void fwd<T>::foo<true>()
{
	printf("true\n");
}

void time_smart_ptr_test()
{
	std::vector<CIntrusivePtr<CTest,CTimeIntrusiveBase> >	test;
	CTest									*t = new CTest();
	CIntrusivePtr<CTest,CTimeIntrusiveBase>	a = t, b = t;
	delete_data								(a);
	fwd<int>::foo<true>();
	fwd<int>::foo<false>();
//	a=b=0;
//	CTimeSmartPtr<CTest>					a = t, b = t;
//	for (u32 i=0; i<100; ++i)
//		test.push_back						(a);//i % 2 ? a : new CTest());
	test.clear								();
}