#include "stdafx.h"
#include "xrCore.h"
#undef STATIC_CHECK
#include <typelist.h>
#include <hierarchygenerators.h>
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

template <typename _type, typename _base>
struct CLinearRegistryType : public _base, public _type
{
};

template <typename T, typename Head>
struct CRegistryHelperLoad {
	template <bool loadable>
	static void do_load(T *self)
	{
	}

	template <>
	static void do_load<true>(T *self)
	{
		self->Head::load();
	}

	static void process(T *self)
	{
		do_load<boost::is_base_and_derived<IPureALifeLObject,Head>::value>(self);
	}
};

template <typename T, typename Head>
struct CRegistryHelperSave {
	template <bool loadable>
	static void do_save(T *self)
	{
	}

	template <>
	static void do_save<true>(T *self)
	{
		self->Head::save();
	}

	static void process(T *self)
	{
		do_save<boost::is_base_and_derived<IPureALifeSObject,Head>::value>(self);
	}
};

template <template <typename _1, typename _2> class helper, typename T, typename TList>
class CRegistryHelperProcess
{
private:
	ASSERT_TYPELIST(TList);

	typedef typename TList::Head Head;
	typedef typename TList::Tail Tail;

public:
	template <typename T1>
	static void go_process(T *self)
	{
		CRegistryHelperProcess<helper,T,Tail>::process(self);
	}

	template <>
	static void go_process<Loki::NullType>(T *self)
	{
	}

	static void process(T *self)
	{
		go_process<Tail>(self);
		helper<T,Head>::process(self);
	}
};

class CRegistryContainer : public Loki::GenLinearHierarchy<final,CLinearRegistryType>::LinBase
{
public:
	template <typename T>
	T &registry(const T*)
	{
		const int value = Loki::TL::IndexOf<final,T>::value;
		STATIC_CHECK(value != -1,There_is_no_specified_registry_in_the_registry_container);
		return	(*static_cast<T*>(this));
	}

	virtual void load()
	{
		CRegistryHelperProcess<CRegistryHelperLoad,CRegistryContainer,final>::process(this);
	}

	virtual void save()
	{
		CRegistryHelperProcess<CRegistryHelperSave,CRegistryContainer,final>::process(this);
	}
};

void registry_test()
{
	CRegistryContainer		registry_container;
	registry_container.load	();
	registry_container.save	();
}
