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

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

class CPatternData {
private:
	char		m_table[256];
	LPCSTR		m_pattern;
	int			m_length;

public:
				CPatternData	(LPCSTR pattern)
	{
		m_pattern		= pattern;
		m_length		= (int)strlen(m_pattern);

		for (int i=0; i<256; ++i)
			m_table[i]	= (char)m_length;

		for (int i=m_length - 1; i>=0; --i)
			if ((int)m_table[m_pattern[i]] == m_length)
				m_table[m_pattern[i]] = char(m_length - i - 1);
	}

	IC	LPCSTR	search			(LPCSTR str, int length, int start = 0) const
	{
		int				pos = start + m_length - 1;
		while (pos < length) {
			if (m_pattern[m_length - 1] != str[pos]) {
				pos		+= m_table[str[pos]];
				continue;
			}
			for (int i = m_length - 2; i>=0; --i) {
				if (m_pattern[i] != str[pos - m_length + i + 1]) {
					++pos;
					break;
				}
				if (!i)
					return	(str + (pos - m_length + 1));
			}
		}
		return				(0);
	}
};

const int TEST_COUNT = 1000000;

void string_test()
{
	Core._initialize("LUA_TEST");

	LPCSTR			str = "_google_yahoo_andex";

	CPatternData	google	("google");
	CPatternData	yahoo	("yahoo");
	CPatternData	yandex	("yandex");

	u64				start, finish;
	
	start			= CPU::GetCycleCount();

	for (int i=0; i<TEST_COUNT; ++i) {
		LPCSTR		r_google = strstr(str,"google");
		LPCSTR		r_yahoo  = strstr(str,"yahoo");
		LPCSTR		r_yandex = strstr(str,"yandex");

		printf		("",r_google,r_yahoo,r_yandex,i);
	}

	finish			= CPU::GetCycleCount();
	
	printf			("strstr : %f\n",(finish-start)*CPU::cycles2milisec);

	start			= CPU::GetCycleCount();

	for (int i=0; i<TEST_COUNT; ++i) {
		int			length = (int)strlen(str);
		LPCSTR		b_google = google.search(str,length);
		LPCSTR		b_yahoo  = yahoo.search	(str,length);
		LPCSTR		b_yandex = yandex.search(str,length);

		printf		("",b_google,b_yahoo,b_yandex,i);
	}

	finish			= CPU::GetCycleCount();
	
	printf			("search : %f\n",(finish-start)*CPU::cycles2milisec);
}