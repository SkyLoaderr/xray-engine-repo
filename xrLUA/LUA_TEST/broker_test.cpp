#include "stdafx.h"	

#pragma warning(push)
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#include "xrCore.h"
#pragma warning(pop)

#include "object_broker.h"
#include <hash_set>
#include <hash_map>

u32 global = 0;
u32 constructor_count = 0;
u32 update_count = 0;
u32 save_count = 0;
u32 inequality_count = 0;

struct CTestObject : public IPureServerObject, public IPureDestroyableObject{
	int a;

	IC								CTestObject	()
	{
		printf	("Constructor called\n");
		++constructor_count;
		a							= global++;
	}

	virtual							~CTestObject	()
	{
		printf	("Destructor called\n");
		--constructor_count;
	}

	IC								CTestObject	(const CTestObject &self)
	{
		++constructor_count;
		this->a = self.a;
	}

	IC		CTestObject				&operator=	(const CTestObject &self)
	{
		this->a						= self.a;
		return						(*this);
	}

	IC		bool					operator==(const CTestObject &self) const
	{
		return						(a == self.a);
	}

	IC		bool					operator!=(const CTestObject &self) const
	{
		return						(a != self.a);
	}

	virtual void					STATE_Write	(NET_Packet &tNetPacket)
	{
		printf	("STATE_Write called\n");
	}

	virtual void					STATE_Read	(NET_Packet &tNetPacket, u16 size)
	{
		printf	("STATE_Read called\n");
	}

	virtual void					UPDATE_Write(NET_Packet &tNetPacket)
	{
		printf	("UPDATE_Write called\n");
		++update_count;
		tNetPacket.w				(&a,sizeof(a));
	}

	virtual void					UPDATE_Read	(NET_Packet &tNetPacket)
	{
		printf	("UPDATE_Read called\n");
		--update_count;
		tNetPacket.r				(&a,sizeof(a));
	}

	virtual void					save		(IWriter	&tMemoryStream)
	{
		printf	("save called\n");
		++save_count;
		tMemoryStream.w				(&a,sizeof(a));
	}

	virtual void					load		(IReader	&tFileStream)
	{
		printf	("load called\n");
		--save_count;
		tFileStream.r				(&a,sizeof(a));
	}

	virtual void					destroy		()
	{
		printf	("destroy called\n");
	}

};

//#define USE_WRITER
//#define USE_REF_STR

#define verify(data) {\
	bool						equality = equal(data,_##data);\
	printf						("%s\n",equality ? "TRUE" : "FALSE");\
	inequality_count			+= equality ? 0 : 1;\
}

#ifndef USE_WRITER
#	define load(data) {\
		load_data					(_##data,packet);\
		verify						(data);\
}
#	define save(data) {\
		save_data					(data,packet);\
}
#else
#	define load(data) {\
		load_data					(_##data,packet);\
		verify						(data);\
		load_data					(__##data,reader);\
		verify						(_##data);\
}
#	define save(data) {\
		save_data					(data,packet);\
		save_data					(data,writer);\
}
#endif

#define _clone(data) {\
	clone(data,___##data);\
	verify(data,___##data);\
}

#define _delete(data) {\
	delete_data					(data);\
	delete_data					(_##data);\
	delete_data					(__##data);\
	delete_data					(___##data);\
}


void traits_test();

struct CMapPredicate {
//	std::hash_multimap<u16,CTestObject*>::iterator  I;

	CMapPredicate(std::hash_multimap<u16,CTestObject*> &data)
	{
//		I = data.begin();
	}

//	template <typename T1>
//	IC	void operator()	(T1 &data) const {}
	template <typename T1, typename T2>
	IC	bool operator()	(T1 &data, const T2 &value) const {return(true);}
	template <typename T1, typename T2>
	IC	bool operator()	(T1 &data, const T2 &value, bool first) const
	{
		return(!first);
	}

	template <typename T1>
	IC	bool operator()	(T1 &data, u16 &value, bool first) const
	{
		if (first)
			value	= 1;
		return		(false);
	}

	IC	void operator()	(std::pair<const u16,CTestObject*> &data) const
	{
		data;
	}
};

#ifdef USE_WRITER
#	undef  INIT_CORE
#	define INIT_CORE
#endif

#ifdef USE_REF_STR
#	undef  INIT_CORE
#	define INIT_CORE
#endif

template <typename T>
struct CTemplate {};

#define TEST_SAVE_LOAD_DELETE

void broker_test()
{
	printf	("%s : %d\n",typeid(xr_vector<u16>).name(),object_type_traits::is_stl_container<xr_multimap<u16,u16> >::value);
//	printf	("%s : %d\n",typeid(CTemplate<u16>).name(),object_type_traits::has_iterator<CTemplate<u16> >::value);
	traits_test();
#ifdef TEST_SAVE_LOAD_DELETE
	{
#ifdef INIT_CORE
		Core._initialize			("lua-test",NULL);
#endif

		LPCSTR						saved_file_name = "x:\\broker_test.dat";

		NET_Packet					packet;
		u16							_packet_type = 6, __packet_type;
		packet.w_begin				(_packet_type);

//////////////////////////////////////////////////////////////////////////
// SAVE TESTS
//////////////////////////////////////////////////////////////////////////

#ifdef USE_WRITER
		CMemoryWriter				writer;
#endif

		// simple type
		u16							_type = 1, __type, ___type, ____type;
		save						(_type);
		// LPCSTR
		LPCSTR						_lpcstr = "LPCSTR", ____lpcstr = 0;
		LPSTR						__lpcstr = 0, ___lpcstr = 0;
		save						(_lpcstr);
		// LPSTR
		LPSTR						_lpstr = xr_strdup("LPSTR"), __lpstr = 0, ___lpstr = 0, ____lpstr = 0;
		save						(_lpstr);
#ifndef USE_WRITER
#ifdef  USE_REF_STR
		// ref_str
		ref_str						_ref_str = "ref_str", __ref_str, ___ref_str, ____ref_str;
		save						(_ref_str);
#endif
#endif
		// savable object
		CTestObject					_object, __object, ___object, ____object;
		save						(_object);
		// pointer to savable object
		CTestObject					*_pobject = xr_new<CTestObject>(), *__pobject = 0, *___pobject = 0, *____pobject = 0;
		save						(_pobject);
		// vector bool
		xr_vector<bool>				_vectorbool, __vectorbool, ___vectorbool, ____vectorbool;
		_vectorbool.push_back		(true);
		save						(_vectorbool);
		// svector
		svector<CTestObject*,5>		_svector, __svector, ___svector, ____svector;
		_svector.push_back			(xr_new<CTestObject>());
		save						(_svector);
		// vector
		xr_vector<CTestObject*>		_vector, __vector, ___vector, ____vector;
		_vector.push_back			(xr_new<CTestObject>());
		save						(_vector);
		// list
		xr_list<CTestObject*>		_list, __list, ___list, ____list;
		_list.push_back				(xr_new<CTestObject>());
		save						(_list);
		// deque
		xr_deque<CTestObject*>		_deque, __deque, ___deque, ____deque;
		_deque.push_back			(xr_new<CTestObject>());
		save						(_deque);
		// queue
		std::queue<CTestObject*>	_queue, __queue, ___queue, ____queue;
		_queue.push					(xr_new<CTestObject>());
		save						(_queue);
		// priority queue
		std::priority_queue<CTestObject*>	_priorityqueue, __priorityqueue, ___priorityqueue, ____priorityqueue;
		_priorityqueue.push			(xr_new<CTestObject>());
		save						(_priorityqueue);
		// stack
		xr_stack<CTestObject*>		_stack, __stack, ___stack, ____stack;
		_stack.push					(xr_new<CTestObject>());
		save						(_stack);
		// set
		xr_set<CTestObject*>		_set, __set, ___set, ____set;
		_set.insert					(xr_new<CTestObject>());
		save						(_set);
		// map
		xr_map<u16,CTestObject*>	_map, __map, ___map, ____map;
		_map.insert					(std::make_pair(1,xr_new<CTestObject>()));
		save						(_map);
		// multiset
		xr_multiset<CTestObject*>	_multiset, __multiset, ___multiset, ____multiset;
		_multiset.insert			(xr_new<CTestObject>());
		save						(_multiset);
		// multimap
		xr_multimap<u16,CTestObject*> _multimap, __multimap, ___multimap, ____multimap;
		_multimap.insert			(std::make_pair(1,xr_new<CTestObject>()));
		save						(_multimap);
		// hashset
		std::hash_set<CTestObject*>	_hashset, __hashset, ___hashset, ____hashset;
		_hashset.insert				(xr_new<CTestObject>());
		save						(_hashset);
		// hashmap
		std::hash_map<u16,CTestObject*>	_hashmap, __hashmap, ___hashmap, ____hashmap;
		_hashmap.insert				(std::make_pair(1,xr_new<CTestObject>()));
		save						(_hashmap);
		// hashmultiset
		std::hash_multiset<CTestObject*>	_hashmultiset, __hashmultiset, ___hashmultiset, ____hashmultiset;
		_hashmultiset.insert		(xr_new<CTestObject>());
		save						(_hashmultiset);
		// hashmultimap
		std::hash_multimap<u16,CTestObject*>	_hashmultimap, __hashmultimap, ___hashmultimap, ____hashmultimap;
		_hashmultimap.insert		(std::make_pair(1,xr_new<CTestObject>()));
		save						(_hashmultimap);
//		save_data					(_hashmultimap,packet,CMapPredicate(_hashmultimap));
#ifdef USE_WRITER
//		save_data					(_hashmultimap,writer,CMapPredicate(_hashmultimap));
#endif

		// complex structure
		typedef xr_vector<bool>			T0;
		typedef svector<T0,5>			T1;
		typedef xr_vector<T1>			T2;
		typedef xr_list<T2>				T3;
		typedef xr_deque<T3>			T4;
		typedef std::queue<T4>			T5;
		typedef std::priority_queue<T5*>T6;
		typedef xr_stack<T6>			T7;
		typedef xr_set<T7*>				T8;
		typedef xr_map<u16,T8>			T9;
		typedef xr_multiset<T9*>		T10;
		typedef xr_multimap<u16,T10>	T11;

		T0							complex0;
		complex0.push_back			(true);
		complex0.push_back			(false);

		T1							complex1;
		complex1.push_back			(complex0);

		T2							complex2;
		complex2.push_back			(complex1);

		T3							complex3;
		complex3.push_back			(complex2);

		T4							complex4;
		complex4.push_back			(complex3);
		
		T5							complex5;
		complex5.push				(complex4);

		T6							complex6;
		complex6.push				(xr_new<T5>(complex5));

		T7							complex7;
		complex7.push				(complex6);

		T8							complex8;
		complex8.insert				(xr_new<T7>(complex7));

		T9							complex9;
		complex9.insert				(std::make_pair(9,complex8));

		T10							complex10;
		complex10.insert			(xr_new<T9>(complex9));

		T11							_complex, __complex, ___complex, ____complex;
		_complex.insert				(std::make_pair(11,complex10));

		save						(_complex);

#ifdef USE_WRITER
		writer.save_to				(saved_file_name);
#endif

//////////////////////////////////////////////////////////////////////////
// LOAD TESTS
//////////////////////////////////////////////////////////////////////////
#ifdef USE_WRITER
		IReader						*preader = FS.r_open(saved_file_name), &reader = *preader;
#endif

		packet.r_begin				(__packet_type);
		R_ASSERT					(_packet_type == __packet_type);

		// simple type
		load						(_type);
		// LPCSTR
		load						(_lpcstr);
		// LPSTR
		load						(_lpstr);
#ifndef USE_WRITER
#ifdef  USE_REF_STR
		// ref_str
		load						(_ref_str);
#endif
#endif
		// savable object
		load						(_object);
		// pointer to savable object
		load						(_pobject);
		// vector bool
		load						(_vectorbool);
		// svector
		load						(_svector);
		// vector
		load						(_vector);
		// list
		load						(_list);
		// deque
		load						(_deque);
		// queue
		load						(_queue);
		// priority queue
		load						(_priorityqueue);
		// stack
		load						(_stack);
		// set
		load						(_set);
		// map
		load						(_map);
		// multiset
		load						(_multiset);
		// multimap
		load						(_multimap);
		// hashset
		load						(_hashset);
		// hashmap
		load						(_hashmap);
		// hashmultiset
		load						(_hashmultiset);
		// hashmultimap
		load						(_hashmultimap);
//		load_data					(__hashmultimap,packet,CMapPredicate(__hashmultimap));
//		verify						(_hashmultimap);
#ifdef USE_WRITER
//		load_data					(___hashmultimap,reader,CMapPredicate(__hashmultimap));
//		verify						(__hashmultimap);
#endif
		// complex
		load						(_complex);

#ifdef USE_WRITER
		FS.r_close					(preader);
#endif

//////////////////////////////////////////////////////////////////////////
// CLONE TESTS
//////////////////////////////////////////////////////////////////////////

		// simple type
		_clone						(_type);
		// LPCSTR
		_clone						(_lpcstr);
		// LPSTR
		_clone						(_lpstr);
#ifndef USE_WRITER
#ifdef  USE_REF_STR
		// ref_str
		_clone						(_ref_str);
#endif
#endif
		// savable object
		_clone						(_object);
		// pointer to savable object
		_clone						(_pobject);
		// vector bool
		_clone						(_vectorbool);
		// svector
		_clone						(_svector);
		// vector
		_clone						(_vector);
		// list
		_clone						(_list);
		// deque
		_clone						(_deque);
		// queue
		_clone						(_queue);
		// priority queue
		_clone						(_priorityqueue);
		// stack
		_clone						(_stack);
		// set
		_clone						(_set);
		// map
		_clone						(_map);
		// multiset
		_clone						(_multiset);
		// multimap
		_clone						(_multimap);
		// hashset
		_clone						(_hashset);
		// hashmap
		_clone						(_hashmap);
		// hashmultiset
		_clone						(_hashmultiset);
		// hashmultimap
		_clone						(_hashmultimap);
		// complex
		_clone						(_complex);

//////////////////////////////////////////////////////////////////////////
// DELETE TESTS
//////////////////////////////////////////////////////////////////////////

		// simple type
		_delete						(_type);
		// LPCSTR
		_delete						(_lpcstr);
		// LPSTR
		_delete						(_lpstr);
#ifndef USE_WRITER
#ifdef  USE_REF_STR
		// ref_str
		_delete						(_ref_str);
#endif
#endif
		// savable object
		_delete						(_object);
		// pointer to savable object
		_delete						(_pobject);
		// vector bool
		_delete						(_vectorbool);
		// svector
		_delete						(_svector);
		// vector
		_delete						(_vector);
		// list
		_delete						(_list);
		// deque
		_delete						(_deque);
		// queue
		_delete						(_queue);
		// priority queue
		_delete						(_priorityqueue);
		// stack
		_delete						(_stack);
		// set
		_delete						(_set);
		// map
		_delete						(_map);
		// multiset
		_delete						(_multiset);
		// multimap
		_delete						(_multimap);
		// hashset
		_delete						(_hashset);
		// hashmap
		_delete						(_hashmap);
		// hashmultiset
		_delete						(_hashmultiset);
		// hashmultimap
		_delete						(_hashmultimap);
		// complex
		_delete						(_complex);
	}

	VERIFY							(!constructor_count);
	VERIFY							(!inequality_count);
	VERIFY							(!update_count);
	VERIFY							(!save_count);
#endif // TEST_SAVE_LOAD_DELETE
}

#define pointer_type(a)				typeid(a).name(),object_type_traits::is_pointer<a >::value

void traits_test()
{
//	printf	("%s : %d\n",pointer_type(char*));
//	printf	("%s : %d\n",pointer_type(char));
//	printf	("%s : %d\n",pointer_type(xr_set<CTestObject*>*));
//	printf	("%s : %d\n",pointer_type(object_type_traits::remove_pointer<char**>::type));
//	printf	("%s : %d\n",pointer_type(object_type_traits::remove_const<const char>::type));
//	printf	("%d\n",object_type_traits::is_base_and_derived<IPureServerObject,CTestObject>::value);
//	printf	("%d\n",object_type_traits::is_base_and_derived<CTestObject,IPureServerObject>::value);
//	printf	("%d\n",object_type_traits::is_base_and_derived<CTestObject,CTestObject>::value);
//	printf	("%d\n",object_type_traits::is_same<IPureServerObject,CTestObject>::value);
//	printf	("%d\n",object_type_traits::is_same<CTestObject,CTestObject>::value);
}