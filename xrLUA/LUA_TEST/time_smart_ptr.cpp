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

typedef CIntrusivePtr<CTest,CTimeIntrusiveBase> intrusive_ptr;

intrusive_ptr foo(intrusive_ptr ptr)
{
	return ptr;
}

void time_smart_ptr_test()
{
	std::vector<intrusive_ptr>	test;
	CTest						*t = new CTest();
//	intrusive_ptr				a = t, b = t;
	foo							(t);
//	delete_data					(a);
	test.clear					();
}
