#include "stdafx.h"
#include <stdio.h>

class Delegate
{
public:
    template<typename T> Delegate(T* pObject, void (T::*pMemberFunction)())
        : pUnknownObject_(pObject)
        , pUnknownMemberFunction_(reinterpret_cast<UnknownMemberFunction>(pMemberFunction))
    {
		pProxyFunction_ = proxy<T>;
	}

    void operator()()
    {
        pProxyFunction_(pUnknownObject_, pUnknownMemberFunction_);
    }
private:
    void* pUnknownObject_;

    class Unknown {};
    typedef void (Unknown::*UnknownMemberFunction)();
    UnknownMemberFunction pUnknownMemberFunction_;

    void (*pProxyFunction_)(void*, UnknownMemberFunction);

    template<typename T> static void proxy(void* pUnknownObject, UnknownMemberFunction pUnknownMemberFunction)
    {
        typedef void (T::*MemberFunction)();

        T* pObject = static_cast<T*>(pUnknownObject);
        MemberFunction pMemberFunction = reinterpret_cast<MemberFunction>(pUnknownMemberFunction);

        (pObject->*pMemberFunction)();
    }
};

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
	B			c;
	Delegate	delegate(&c,&B::test);
	delegate	();
}
