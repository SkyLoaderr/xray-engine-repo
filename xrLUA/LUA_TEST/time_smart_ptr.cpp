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

#undef STATIC_CHECK
#include <typelist.h>

#pragma comment(lib,"Winmm.lib")

struct CTest : public CTimeIntrusiveBase {
	virtual ~CTest()
	{
		printf			("CTest::~CTest();");
	}
};

typedef CIntrusivePtr<CTest,CTimeIntrusiveBase> intrusive_ptr;

intrusive_ptr foo(intrusive_ptr ptr)
{
	return ptr;
}

struct C{};

struct D : public C {};

template <typename _1>
struct B{};

struct A : public B<C> {
};

void time_smart_ptr_test()
{
	printf						("%s\n",object_type_traits::is_base_and_derived_or_same_for_template_template_1_1<B,A,C>::value ? "TRUE" : "FALSE");
	printf						("%s\n",object_type_traits::is_base_and_derived_or_same_for_template_template_1_1<B,B<C>,D>::value ? "TRUE" : "FALSE");
	printf						("%s\n",object_type_traits::is_base_and_derived_or_same_from_template_1<B,A>::value ? "TRUE" : "FALSE");
	printf						("%s\n",object_type_traits::is_base_and_derived_or_same_from_template_1<B,C>::value ? "TRUE" : "FALSE");
	std::vector<intrusive_ptr>	test;
	CTest						*t = new CTest();
//	intrusive_ptr				a = t, b = t;
	foo							(t);
//	delete_data					(a);
	test.clear					();
}
