#include "stdafx.h"
#pragma warning(push)
#pragma warning(disable:4995)
#include "xrCore.h"
#pragma warning(pop)
#undef STATIC_CHECK
#include <typelist.h>
#include <hierarchygenerators.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/type_traits/is_base_and_derived.hpp>

__interface IPureALifeLObject {
public:
	virtual void					load()						= 0;
};

__interface IPureALifeSObject {
public:
	virtual void					save()						= 0;
};

__interface IPureALifeLSObject : public IPureALifeLObject, public IPureALifeSObject {
};

__interface IPureALifeXObject {
	virtual void					foo(int a, int b)			= 0;
};

struct TestLoad : public IPureALifeLObject {
	virtual ~TestLoad()
	{
		printf						("TestLoad::~TestLoad() called\n");
	}

	virtual void					load()
	{
		printf						("TestLoad::load() called\n");
	}
};

struct TestSave : public IPureALifeSObject {
	virtual ~TestSave()
	{
		printf						("TestSave::~TestSave() called\n");
	}

	virtual void					save()
	{
		printf						("TestSave::save() called\n");
	}
};

struct TestLoadSave : public IPureALifeLSObject {
	virtual ~TestLoadSave()
	{
		printf						("TestLoadSave::~TestLoadSave() called\n");
	}

	virtual void					load()
	{
		printf						("TestLoadSave::load() called\n");
	}
	
	virtual void					save()
	{
		printf						("TestLoadSave::save() called\n");
	}
};

struct TestClear {
	virtual ~TestClear()
	{
		printf						("TestClear::~TestClear() called\n");
	}
};

struct TestClear1 : public TestClear, public IPureALifeXObject {
	virtual ~TestClear1()
	{
		printf						("TestClear::~TestClear() called\n");
	}

	virtual void					foo(int a, int b)
	{
		printf						("TestClear1::foo(%d, %d) called\n",a,b);
	}
};

//#include "alife_abstract_registry.h"

#define final Loki::NullType
/**
#define ADD(a,b) \
	class CRegistryHelper##a : public a {\
	public:			a &b()			{return(*this);}\
	public :   const	a &b() const	{return(*this);}\
	};\
	typedef Loki::Typelist<CRegistryHelper##a,final> registry_##a;
/**/
#define ADD(a,b) \
	typedef Loki::Typelist<a,final> registry_##a;\
	const a* b = 0;

#define SAVE(a) registry_##a

//typedef CALifeAbstractRegistry<u8,  std::vector<u8> >  r8;
//typedef CALifeAbstractRegistry<u16, std::vector<u16> > r16;
//typedef CALifeAbstractRegistry<u32, std::vector<u32> > r32;
//typedef CALifeAbstractRegistry<u64, std::vector<u64> > r64;

//namespace {
//	const r8 *InfoPortions = 0;
//};

//#define InfoPortions (r8*)0
//typedef r8 CInfoPortions;

//	registry_container.get_r16().add(1,std::vector<u16>());

//	registry_container.get_r8().add	(1,std::vector<u8>());

//	registry_container.registry<CInfoPortions>().add	(2,std::vector<u8>());

//	registry_container.get<eR8_Registry>().add	(2,std::vector<u8>());
//	registry_container.get((r8*)0).add	(2,std::vector<u8>());

//	registry_container.registry(info_portions).add	(0,std::vector<u8 >());
//	registry_container.registry(tasks).add			(0,std::vector<u16>());
//	registry_container.registry(dairy).add			(0,std::vector<u32>());
//	registry_container.registry(relations).add		(0,std::vector<u64>());
//	registry_container

typedef TestLoad		r8;
typedef TestSave		r16;
typedef TestLoadSave	r32;
typedef TestClear		r64;
typedef TestClear1		r128;

ADD(r8,info_portions)
#undef final
#define final SAVE(r8)

ADD(r16,tasks)
#undef final
#define final SAVE(r16)

ADD(r32,dairy)
#undef final
#define final SAVE(r32)

ADD(r64,relations)
#undef final
#define final SAVE(r64)

ADD(r128,relations1)
#undef final
#define final SAVE(r128)

template <typename _type>
class CLinearRegistryTypeBase
{
public:
	typedef _type		REGISTRY;

protected:
	REGISTRY			*m_registry;

public:
	IC					CLinearRegistryTypeBase	()
	{
		m_registry		= new REGISTRY();
	}

	virtual				~CLinearRegistryTypeBase()
	{
		delete			(m_registry);
	}

	IC		REGISTRY	&registry				(REGISTRY *) const
	{
		return			(*m_registry);
	}
};

template <typename _type, typename _base>
class CLinearRegistryType : public _base, public CLinearRegistryTypeBase<_type>
{
public:
	using CLinearRegistryTypeBase<_type>::registry;
	using _base::registry;
};

template <typename _type>
class CLinearRegistryType<_type,Loki::EmptyType> : public Loki::EmptyType, public CLinearRegistryTypeBase<_type>
{
public:
	using CLinearRegistryTypeBase<_type>::registry;
};

template <typename T1, typename T2, typename T3, typename TList>
class CRegistryHelper
{
private:
	ASSERT_TYPELIST(TList);

	typedef typename TList::Head Head;
	typedef typename TList::Tail Tail;

	template <typename T1, typename T2, typename T3, typename Head>
	struct CRegistryHelperCall {
		template <bool is_callable>
		static void do_call(T1 *self, const T2 &p2)
		{
		}

		template <>
		static void do_call<true>(T1 *self, const T2 &p2)
		{
			p2(self->registry((Head*)0));
		}

		static void process(T1 *self, const T2 &p2)
		{
			do_call<boost::is_base_and_derived<T3,Head>::value>(self,p2);
		}
	};

public:
	template <typename T>
	static void go_process(T1 *self, const T2 &p2)
	{
		CRegistryHelper<T1,T2,T3,Tail>::process(self,p2);
	}

	template <>
	static void go_process<Loki::NullType>(T1 *self, const T2 &p2)
	{
	}

	static void process(T1 *self, const T2 &p2 = T2())
	{
		go_process<Tail>(self,p2);
		CRegistryHelperCall<T1,T2,T3,Head>::process(self,p2);
	}
};

template <typename func_type>
class CCaller {
private:
	func_type f;

public:
	IC				CCaller		(const func_type &_f) 
									: f(_f)
	{
	}

	template <typename T1>
	IC		void	operator()	(T1 &p1) const
	{
		f(&p1);
	}
};

template <typename T0, typename T1, typename T2, typename T3>
void hierarchy_call(T3 *self, const T2 &caller)
{
	typedef boost::function<T0 (T1*)> func_type;
	CRegistryHelper<T3,CCaller<func_type>,T1,final>::process(self,CCaller<func_type>(func_type(caller)));
}

class CRegistryContainer : public Loki::GenLinearHierarchy<final,CLinearRegistryType>::LinBase
{
public:
	
	virtual void load()
	{
		hierarchy_call<void,IPureALifeLObject>(this,IPureALifeLObject::load);
	}

	virtual void save()
	{
		hierarchy_call<void,IPureALifeSObject>(this,IPureALifeSObject::save);
	}

	virtual void foo(int a, int b)
	{
		hierarchy_call<void,IPureALifeXObject>(this,boost::bind(IPureALifeXObject::foo,_1,a,b));
	}
};

void abstract_registry_test()
{
	CRegistryContainer			registry_container;
	registry_container.load		();
	registry_container.save		();
	registry_container.foo		(6,7);
	hierarchy_call<void,IPureALifeXObject>(&registry_container,boost::bind(IPureALifeXObject::foo,_1,8,9));

	registry_container.registry	((r8*)0);
	registry_container.registry	((r16*)0);
	registry_container.registry	((r32*)0);
	registry_container.registry	((r64*)0);
	registry_container.registry	((r128*)0);
}
