#include "stdafx.h"	

#pragma warning(push)
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#include "xrCore.h"
#pragma warning(pop)

#include "object_broker.h"

#include "net_utils.h"

#include <hash_set>
#include <hash_map>

u32 global = 0;
u32 constructor_count = 0;
u32 update_count = 0;
u32 save_count = 0;
u32 inequality_count = 0;

struct CTestObject : public IPureServerObject {
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
		this->a = self.a;
	}

	IC		bool					operator==(const CTestObject &self) const
	{
		return						(a == self.a);
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
};

struct CEqualizer {
	template <typename T>
	struct CHelper {
		template <bool a>
		IC	static bool equal(const T &_1, const T &_2)
		{
			return			(_1 == _2);
		}

		template <>
		IC	static bool equal<true>(const T &_1, const T &_2)
		{
			return			(CEqualizer::equal(*_1,*_2));
		}
	};

	template <typename T>
	IC	static bool equal(const T &_1, const T &_2)
	{
		return						(CHelper<T>::equal<boost::is_pointer<T>::value>(_1,_2));
	}

	IC	static bool equal(LPCSTR &_1, LPCSTR &_2)
	{
		return						(!xr_strcmp(_1,_2));
	}

	IC	static bool equal(LPSTR &_1, LPSTR &_2)
	{
		return						(!xr_strcmp(_1,_2));
	}

	IC	static bool equal(ref_str &_1, ref_str &_2)
	{
		return						(!xr_strcmp(_1,_2));
	}

	template <typename T1, typename T2>
	IC	static bool equal(const std::pair<T1,T2> &_1, const std::pair<T1,T2> &_2)
	{
		return						(
			equal(_1.first,_2.first)
			&&
			equal(_1.second,_2.second)
		);
	}

	template <typename T, int size>
	IC	static bool equal(const svector<T,size> &__1, const svector<T,size> &__2)
	{
		svector<T,size>				&_1 = *const_cast<svector<T,size>*>(&__1);
		svector<T,size>				&_2 = *const_cast<svector<T,size>*>(&__2);
		if (_1.size() != _2.size())
			return					(false);

		svector<T,size>::iterator	I = _1.begin(), J = _2.begin();
		svector<T,size>::iterator	E = _1.end();
		for ( ; I != E; ++I, ++J)
			if (!equal(*I,*J))
				return				(false);
		return						(true);
	}

	template <typename T1, typename T2>
	IC	static bool equal(const std::queue<T1,T2> &__1, const std::queue<T1,T2> &__2)
	{
		std::queue<T1,T2>			_1 = __1;
		std::queue<T1,T2>			_2 = __2;
		
		if (_1.size() != _2.size())
			return					(false);

		for ( ; !_1.empty(); _1.pop(), _2.pop())
			if (!equal(_1.front(),_2.front()))
				return				(false);
		return						(true);
	}

	template <template <typename _1, typename _2> class T1, typename T2, typename T3>
	IC	static bool equal(const T1<T2,T3> &__1, const T1<T2,T3> &__2, bool)
	{
		T1<T2,T3>					_1 = __1;
		T1<T2,T3>					_2 = __2;

		if (_1.size() != _2.size())
			return					(false);

		for ( ; !_1.empty(); _1.pop(), _2.pop())
			if (!equal(_1.top(),_2.top()))
				return				(false);
		return						(true);
	}

	template <template <typename _1, typename _2, typename _3> class T1, typename T2, typename T3, typename T4>
	IC	static bool equal(const T1<T2,T3,T4> &__1, const T1<T2,T3,T4> &__2, bool)
	{
		T1<T2,T3,T4>				_1 = __1;
		T1<T2,T3,T4>				_2 = __2;

		if (_1.size() != _2.size())
			return					(false);

		for ( ; !_1.empty(); _1.pop(), _2.pop())
			if (!equal(_1.top(),_2.top()))
				return				(false);
		return						(true);
	}

	template <typename T1, typename T2>
	IC	static bool equal(const xr_stack<T1,T2> &_1, const xr_stack<T1,T2> &_2)
	{
		return					(equal(_1,_2,true));
	}

	template <typename T1, typename T2, typename T3>
	IC	static bool equal(const std::priority_queue<T1,T2,T3> &_1, const std::priority_queue<T1,T2,T3> &_2)
	{
		return					(equal(_1,_2,true));
	}

	template <template <typename _1> class T1, typename T2>
	IC	static bool equal(const T1<T2> &_1, const T1<T2> &_2)
	{
		if (_1.size() != _2.size())
			return					(false);

		T1<T2>::const_iterator			I = _1.begin(), J = _2.begin();
		T1<T2>::const_iterator			E = _1.end();
		for ( ; I != E; ++I, ++J)
			if (!equal(*I,*J))
				return					(false);
		return							(true);
	}

	template <template <typename _1, typename _2> class T1, typename T2, typename T3>
	IC	static bool equal(const T1<T2,T3> &_1, const T1<T2,T3> &_2)
	{
		if (_1.size() != _2.size())
			return					(false);

		T1<T2,T3>::const_iterator			I = _1.begin(), J = _2.begin();
		T1<T2,T3>::const_iterator			E = _1.end();
		for ( ; I != E; ++I, ++J)
			if (!equal(*I,*J))
				return					(false);
		return							(true);
	}

	template <template <typename _1, typename _2, typename _3> class T1, typename T2, typename T3, typename T4>
	IC	static bool equal(const T1<T2,T3,T4> &_1, const T1<T2,T3,T4> &_2)
	{
		if (_1.size() != _2.size())
			return					(false);

		T1<T2,T3,T4>::const_iterator		I = _1.begin(), J = _2.begin();
		T1<T2,T3,T4>::const_iterator		E = _1.end();
		for ( ; I != E; ++I, ++J)
			if (!equal(*I,*J))
				return					(false);
		return							(true);
	}

	template <template <typename _1, typename _2, typename _3, typename _4> class T1, typename T2, typename T3, typename T4, typename T5>
	IC	static bool equal(const T1<T2,T3,T4,T5> &_1, const T1<T2,T3,T4,T5> &_2)
	{
		if (_1.size() != _2.size())
			return					(false);

		T1<T2,T3,T4,T5>::const_iterator	I = _1.begin(), J = _2.begin();
		T1<T2,T3,T4,T5>::const_iterator	E = _1.end();
		for ( ; I != E; ++I, ++J)
			if (!equal(*I,*J))
				return					(false);
		return							(true);
	}
};

#define USE_WRITER

#define verify(data) {\
	bool						equality = CEqualizer::equal(data,_##data);\
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

#define _delete(data) {\
	delete_data					(data);\
	delete_data					(_##data);\
	delete_data					(__##data);\
}

void broker_test()
{
	{
		Core._initialize			("lua-test",NULL);

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
		u16							_type = 1, __type, ___type;
		save						(_type);
		// LPCSTR
		LPCSTR						_lpcstr = "LPCSTR";
		LPSTR						__lpcstr = (LPSTR)xr_malloc(xr_strlen(_lpcstr) + 1), ___lpcstr = (LPSTR)xr_malloc(xr_strlen(_lpcstr) + 1);
		save						(_lpcstr);
		// LPSTR
		LPSTR						_lpstr = xr_strdup("LPSTR"), __lpstr = (LPSTR)xr_malloc(xr_strlen(_lpstr) + 1), ___lpstr = (LPSTR)xr_malloc(xr_strlen(_lpstr) + 1);
		save						(_lpstr);
#ifndef USE_WRITER
		// ref_str
		ref_str						_ref_str = "ref_str", __ref_str, ___ref_str;
		save						(_ref_str);
#endif
		// savable object
		CTestObject					_object, __object, ___object;
		save						(_object);
		// pointer to savable object
		CTestObject					*_pobject = xr_new<CTestObject>(), *__pobject = 0, *___pobject = 0;
		_pobject->a					= 2;
		save						(_pobject);
		// vector bool
		xr_vector<bool>				_vectorbool, __vectorbool, ___vectorbool;
		_vectorbool.push_back		(true);
		save						(_vectorbool);
		// svector
		svector<CTestObject*,5>		_svector, __svector, ___svector;
		_svector.push_back			(xr_new<CTestObject>());
		save						(_svector);
		// vector
		xr_vector<CTestObject*>		_vector, __vector, ___vector;
		_vector.push_back			(xr_new<CTestObject>());
		save						(_vector);
		// list
		xr_list<CTestObject*>		_list, __list, ___list;
		_list.push_back				(xr_new<CTestObject>());
		save						(_list);
		// deque
		xr_deque<CTestObject*>		_deque, __deque, ___deque;
		_deque.push_back			(xr_new<CTestObject>());
		save						(_deque);
		// queue
		std::queue<CTestObject*>	_queue, __queue, ___queue;
		_queue.push					(xr_new<CTestObject>());
		save						(_queue);
		// priority queue
		std::priority_queue<CTestObject*>	_priorityqueue, __priorityqueue, ___priorityqueue;
		_priorityqueue.push			(xr_new<CTestObject>());
		save						(_priorityqueue);
		// stack
		xr_stack<CTestObject*>		_stack, __stack, ___stack;
		_stack.push					(xr_new<CTestObject>());
		save						(_stack);
		// set
		xr_set<CTestObject*>		_set, __set, ___set;
		_set.insert					(xr_new<CTestObject>());
		save						(_set);
		// map
		xr_map<u16,CTestObject*>	_map, __map, ___map;
		_map.insert					(std::make_pair(1,xr_new<CTestObject>()));
		save						(_map);
		// multiset
		xr_multiset<CTestObject*>	_multiset, __multiset, ___multiset;
		_multiset.insert			(xr_new<CTestObject>());
		save						(_multiset);
		// multimap
		xr_multimap<u16,CTestObject*> _multimap, __multimap, ___multimap;
		_multimap.insert			(std::make_pair(1,xr_new<CTestObject>()));
		save						(_multimap);
		// hashset
		std::hash_set<CTestObject*>	_hashset, __hashset, ___hashset;
		_hashset.insert				(xr_new<CTestObject>());
		save						(_hashset);
		// hashmap
		std::hash_map<u16,CTestObject*>	_hashmap, __hashmap, ___hashmap;
		_hashmap.insert				(std::make_pair(1,xr_new<CTestObject>()));
		save						(_hashmap);
		// hashmultiset
		std::hash_multiset<CTestObject*>	_hashmultiset, __hashmultiset, ___hashmultiset;
		_hashmultiset.insert		(xr_new<CTestObject>());
		save						(_hashmultiset);
		// hashmultimap
		std::hash_multimap<u16,CTestObject*>	_hashmultimap, __hashmultimap, ___hashmultimap;
		_hashmultimap.insert		(std::make_pair(1,xr_new<CTestObject>()));
		save						(_hashmultimap);

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
		// ref_str
		load						(_ref_str);
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

#ifdef USE_WRITER
		FS.r_close					(preader);
#endif

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
		// ref_str
		_delete						(_ref_str);
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
	}

	VERIFY							(!constructor_count);
	VERIFY							(!inequality_count);
	VERIFY							(!update_count);
	VERIFY							(!save_count);
}
