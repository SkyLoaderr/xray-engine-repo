#include "stdafx.h"
#pragma warning(push)
#pragma warning(disable:4995)
#include "xrCore.h"
#pragma warning(pop)
#undef STATIC_CHECK
#include <boost/function.hpp>
#include <boost/bind.hpp>

struct A {
	void foo(int _1, LPCSTR _2, A *_3)
	{
		printf("Obana!\n");
	}
};
//
//struct CCallbackHolder {
//	typedef boost::function<void ()> functor;
//	
//	enum ECallbackType {
//		eCallbackForce = u32(0),
//		eCallbackCollision,
//		eCallbackDummy = u32(-1),
//	};
//
//	struct ICallbackQueue {
//		virtual void update	() = 0;
//	}
//
//	template <typename _functor>
//	struct CCallbackQueue : public ICallbackQueue {
//		typedef xr_vector<functor> CALLBACKS;
//
//		_functor			m_base;
//		xr_vector<functor>	m_callbacks;
//
//		virtual void update		()
//		{
//			CALLBACKS::iterator	I = m_callbacks.begin();
//			CALLBACKS::iterator	E = m_callbacks.end();
//			for ( ; I != E; ++I)
//				(*I)();
//			m_callbacks.clear	();
//		}
//	}
//
//	typedef xr_map<ECallbackType,ICallbackQueue*> CALLBACKS;
//
//	CALLBACKS	m_callbacks;
//
//	void		update			()
//	{
//		CALLBACKS::iterator	I = m_callbacks.begin();
//		CALLBACKS::iterator	E = m_callbacks.end();
//		for ( ; I != E; ++I)
//			if ((*I).second)
//				(*I).second->update	();
//	}
//
//	void		set_callback	();
//	functor		&callback		(const ECallbackType &callback_type);
//};

#	define	THROW(xpr)				if (!(xpr)) {throw __FILE__LINE__"\""#xpr"\"";}
#	define	THROW2(xpr,msg0)		if (!(xpr)) {throw *shared_str().sprintf("%s \"%s\" : %s",__FILE__LINE__,#xpr,msg0 ? msg0 : "");}
#	define	THROW3(xpr,msg0,msg1)	if (!(xpr)) {throw *shared_str().sprintf("%s \"%s\" : %s, %s",__FILE__LINE__,#xpr,msg0 ? msg0 : "",msg1 ? msg1 : "");}

LPCSTR msg0 = "message0";
LPCSTR msg1 = "message1";

void slipch_test()
{
//	Core._initialize("LUA_TEST");
	A							a;
	boost::function<void (A*,int,LPCSTR,A*)>	_f = &A::foo;
	boost::function<void ()>	f;
	f							= boost::bind(_f,&a,1,"2",(A*)0);
	f							();
//	CCallbackHolder		holder;
//	holder.setup		(eCallbackCollision,);
//	try {
//		THROW(false);
//		THROW2(false,msg0);
//		THROW3(false,"message0",msg1);
//	}
//	catch(LPCSTR msg) {
//		printf("Expression : %s\n",msg);
//	}
}
