#include "stdafx.h"
#include <stdio.h>
#include "FastDelegate.h"

using namespace fastdelegate;

struct A{
	virtual ~A(){}
	void test()
	{
		printf	("A::test\n");
	}
};

struct B{
	virtual ~B(){}
	void test()
	{
		printf	("B::test\n");
	}
};

struct C : public A, public B {
	virtual ~C(){}
	void test()
	{
		printf	("C::test\n");
	}
};

void delegate_test()
{
	B				c;
	FastDelegate0	delegate(&c,&B::test);
	delegate		();
}
