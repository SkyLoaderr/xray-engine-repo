////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_container.h
//	Created 	: 22.10.2004
//  Modified 	: 22.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Smart container class
////////////////////////////////////////////////////////////////////////////

#pragma once

//#define USE_LOKI
// you can uncomment this define, 
// but all the loki routines are already hand-written

//#define USE_BOOST_TRAITS
// you can uncomment this define
// tio prevent warnings about 
// decoration name limit exceeding

#define DEFAULT_POINTER(T)	T*						
// if you'd like not to use pointers just make DEFAULT_POINTER(T) T
// if you'd like to use shared_ptr or intrusive_ptr -> please inform me,
// since this version doesn't support them

#define DEFAULT_CONTAINER	std::vector
// you can use the following containers :
// vector,
// deque,
// list,
// set,
// multiset,
// hash_set,
// hash_multiset
//
// you cannot use the following containers :
// map,
// multimap,
// hash_map,
// hash_multimap
//
// to use the following containers I need to creatre a workaround :
// queue
// stack
// priority_queue
//
// but you still can use object broker templates for all of the metnioned classes :
// save_data
// load_data
// delete_data
// clone
// compare :
//		custom predicate,
//		equal,				
//		not_equal
//		greater_equal,		
//		greater,			
//		less_equal,		
//		less,				
//		completely_inequal,
//		logical_and,		
//		logical_or	

#include "object_broker.h"

#ifdef USE_BOOST_TRAITS
#	include <boost/type_traits/is_base_and_derived.hpp>
#endif

#ifdef USE_LOKI
#	include <typelist.h>
#endif

namespace Loki {
	class EmptyType{};

#ifndef USE_LOKI
	class NullType {
	public:
		struct Head { private: Head(); };
		struct Tail { private: Tail(); };
	};

	template <class T, class U>
	struct Typelist {
		typedef T Head;
		typedef U Tail;
	};

	namespace TL {

		template <class TList, class T> 
		struct Erase
		{
			typedef typename TList::Head Head;
			typedef typename TList::Tail Tail;
	    
		private:
			template<class TList1>
			struct In
			{
				typedef typename TList1::Tail Tail;

				typedef Typelist
				<
					Head, 
					typename Erase<Tail, T>::Result
				>
				Result;
			};

			template<>
			struct In< Typelist<T, Tail> >
			{
				typedef Tail Result;
			};

			template<>
			struct In<NullType>
			{
				typedef NullType Result;
			};

		public:
			typedef typename In<TList>::Result Result;
		};

		template <class TList> 
		struct NoDuplicates
		{
		private:
			typedef typename TList::Head Head;
			typedef typename TList::Tail Tail;
		
			typedef typename NoDuplicates<Tail>::Result L1;
			typedef typename Erase<L1, Head>::Result    L2;

		public:
			typedef Typelist<Head, L2> Result;
		};
		
		template <> 
		struct NoDuplicates<NullType>
		{
			typedef NullType Result;
		};
	};
#endif
};

#define final_vertex_type_list			Loki::NullType
#define vertex_type_add(a)				typedef Loki::Typelist<a,final_vertex_type_list> vertex_type_list_##a;
#define current_vertex_type_list(a)		vertex_type_list_##a

#include "smart_container_types.h"

template <typename T>
struct pointer_type {
	typedef DEFAULT_POINTER(T) result;
};

struct core_container {
	template <typename T>
	struct type {
		typedef typename pointer_type<T>::result	type_value;
		typedef DEFAULT_CONTAINER<type_value>		result;
		typedef typename result::value_type			value_type;
	};

	template <typename T1, typename T2>
	IC	static void add(T1 &data, const T2 &value)
	{
		CLoader<T1,T2>::CHelper3::add(data,value);
	}
};

template <typename T>
struct current_type {
	template <bool>
	struct selector {
		typedef T result;
	};

	template <>
	struct selector<false> {
		typedef T* result;
	};

	typedef typename selector<object_type_traits::is_pointer<T>::value>::result result;
};

template <typename T>
struct return_type {
	template <bool>
	struct selector {
		typedef T result;
	};

	template <>
	struct selector<false> {
		typedef T& result;
	};

	typedef typename selector<object_type_traits::is_pointer<T>::value>::result result;
};

template <typename T1, typename T2>
struct setup_value_helper {
	template <bool>
	static void setup_value(T1 &value, T2 &iterator)
	{
		value = *iterator;
	}

	template <>
	static void setup_value<false>(T1 &value, T2 &iterator)
	{
		value = &*iterator;
	}
};

template <typename T1, typename T2>
void setup_value(T1 &value, T2 &iterator)
{
	setup_value_helper<T1,T2>::setup_value<object_type_traits::is_pointer<typename T2::value_type>::value>(value,iterator);
}

template <typename T2, typename T1>
struct get_value_helper {
	template <bool>
	static T2 get_value(T1 &value)
	{
		return	(value);
	}

	template <>
	static T2 get_value<false>(T1 &value)
	{
		return	(*value);
	}
};

template <typename T2, typename T1>
T2 get_value(T1 &value)
{
	return	(get_value_helper<T2,T1>::get_value<object_type_traits::is_pointer<T2>::value>(value));
}

template <typename T1, typename T2>
struct try_convert_helper {
	template <bool>
	static void try1(T1 *&value, T2 &iterator)
	{
		value	= *iterator;
	}

	template <>
	static void try1<false>(T1 *&value, T2 &iterator)
	{
		value	= &*iterator;
	}

	template <bool>
	static bool try0(T1 *&value, T2 &iterator)
	{
		try1<object_type_traits::is_pointer<typename T2::value_type>::value>(value,iterator);
		return	(true);
	}

	template <>
	static bool try0<false>(T1 *&value, T2 &iterator)
	{
		value	= 0;
		return	(false);
	}
};

template <typename T1, typename T2>
IC	static bool	_try_convert(T1 *&p, T2 &iterator)
{
	typedef typename T2::value_type											value_type;
	typedef typename object_type_traits::remove_pointer<T1>::type			t1_result;
	typedef typename object_type_traits::remove_pointer<value_type>::type	vt_result;

	return		(
		try_convert_helper<T1,T2>::try0<
#ifdef USE_BOOST_TRAITS
			boost::is_base_and_derived<
				t1_result,
				vt_result
			>::value ||
#else
			object_type_traits::is_base_and_derived<
				t1_result,
				vt_result
			>::value ||
#endif
			object_type_traits::is_same<
				t1_result,
				vt_result
			>::value
		>
		(p,iterator)
	);
}

template <typename _vertex_type, typename _container_type = typename core_container::type<_vertex_type>::result>
class vertex_container {
public:
	typedef _vertex_type							_vertex_type;
	typedef _container_type							_container_type;
	typedef typename _container_type::value_type	value_type;
	typedef typename _container_type::iterator		iterator;
private:
	mutable _container_type	m_objects;

public:
	IC				vertex_container	()
	{
	}

	virtual			~vertex_container	()
	{
		clear		();
	}

	IC		void	add					(const value_type &object)
	{
		core_container::add	(m_objects,object);
	}

	IC		void remove					(const value_type &object)
	{
		iterator			I = std::find(m_objects.begin(),m_objects.end(),object);
		if ((I == m_objects.end()))// || (*I != object))
			return;
		m_objects.erase		(I);
	}

	IC	_container_type	&objects		() const
	{
		return		(m_objects);
	}

	virtual void	load				(IReader &stream)
	{
		load_data	(m_objects,stream);
	}

	virtual void	save				(IWriter &stream)
	{
		save_data	(m_objects,stream);
	}

	IC		bool	operator==			(const vertex_container &t) const
	{
		return		(equal(objects(),t.objects()));
	}

	IC		void	clear						()
	{
		delete_data	(m_objects);
	}
};

template <typename _type, typename _base>
struct vertex_hierarchy_helper : public vertex_container<_type>, public _base
{
	using vertex_container<_type>::add;
	using vertex_container<_type>::remove;
	using _base::add;
	using _base::remove;

	virtual			~vertex_hierarchy_helper	(){};

	virtual void	load						(IReader &stream)
	{
		vertex_container<_type>::load	(stream);
		_base::load						(stream);
	}

	virtual void	save						(IWriter &stream)
	{
		vertex_container<_type>::save	(stream);
		_base::save						(stream);
	}

	IC		bool	operator==					(const vertex_hierarchy_helper &t) const
	{
		return		(
			((_base&)*this == t) &&
			((vertex_container<_type>&)*this == t)
		);
	}

	IC		void	clear						()
	{
		((_base*)this)->clear();
		((vertex_container<_type>*)this)->clear();
	}
};

template <typename _type>
struct vertex_hierarchy_helper<_type,Loki::EmptyType> : public vertex_container<_type>, public IPureSerializeObject
{
	using vertex_container<_type>::add;
	using vertex_container<_type>::remove;

	virtual			~vertex_hierarchy_helper	(){};

	virtual void	load						(IReader &stream)
	{
		vertex_container<_type>::load	(stream);
	}

	virtual void	save						(IWriter &stream)
	{
		vertex_container<_type>::save	(stream);
	}

	IC		bool	operator==					(const vertex_hierarchy_helper &t) const
	{
		return		((vertex_container<_type>&)*this == t);
	}

	IC		void	clear						()
	{
		((vertex_container<_type>*)this)->clear();
	}
};

template <typename list, template <typename _1, typename _2> class generator>
struct hierarchy_generator {
	template <typename T>
	struct helper {
		typedef typename T::Head Head;
		typedef typename T::Tail Tail;

		typedef generator<Head,typename helper<Tail>::result> result;
	};

	template <>
	struct helper<Loki::NullType> {
		typedef Loki::EmptyType result;
	};

	typedef typename helper<list>::result result;
};

template <typename type_list>
struct _container : 
	public hierarchy_generator<
		type_list,
		vertex_hierarchy_helper
	>::result
{
	typedef typename hierarchy_generator<
		type_list,
		vertex_hierarchy_helper
	>::result inherited;
	template <typename Type>
	struct derived {
		template <typename T>
		struct helper {
			typedef typename T::Head Head;
			typedef typename T::Tail Tail;

			template <bool>
			struct selector {
				typedef Loki::Typelist<Head,typename helper<Tail>::result> result;
			};

			template <>
			struct selector<false> {
				typedef typename helper<Tail>::result result;
			};

#ifdef USE_BOOST_TRAITS
			typedef typename selector<boost::is_base_and_derived<Type,Head>::value || object_type_traits::is_same<Type,Head>::value>::result result;
#else
			typedef typename selector<object_type_traits::is_base_and_derived<Type,Head>::value || object_type_traits::is_same<Type,Head>::value>::result result;
#endif
		};

		template <>
		struct helper<Loki::NullType> {
			typedef Loki::NullType result;
		};

		typedef typename helper<type_list>::result result;
	};

	virtual ~_container(){}

	template <typename dest_type>
	struct _iterator_helper {
		template <typename _type, typename _base>
		struct iterator_helper : public _base
		{
		public:
			typedef iterator_helper<_type,_base>						self_type;
			typedef Loki::Typelist<self_type,typename _base::self_list> self_list;
			typedef typename core_container::type<_type>::result		result;
			typedef typename result::iterator							iterator;
			typedef _base												inherited;
			typedef typename result::value_type							value_type;
			typedef typename pointer_type<dest_type>::result			value;

		public:
			iterator		m_iterator;
			iterator		m_end;
			bool			m_used;

		public:
			template <typename T>
			IC	bool		init	(const T *holder)
			{
				m_iterator	= holder->container((vertex_container<_type>*)0)->objects().begin();
				m_end		= holder->container((vertex_container<_type>*)0)->objects().end();
				m_used		= false;
				if (!_base::init(holder)) {
					if (m_iterator == m_end)
						return	(false);
					m_used		= true;
					setup_value	(m_current,m_iterator);
				}
				return		(true);
			}

			template <typename T>
			IC	bool		init	(const T *holder, bool end_iterator)
			{
				_base::init	(holder,end_iterator);
				m_iterator	= m_end	= holder->container((vertex_container<_type>*)0)->objects().end();
				m_used		= true;
				return		(false);
			}

			IC				iterator_helper	()
			{
			}

			template <template <typename _> class _predicate>
			IC	static bool compare(const self_type &_1, const self_type &_2)
			{
				return	(
					_predicate<iterator>()(_1.m_iterator,	_2.m_iterator) &&
					_predicate<iterator>()(_1.m_end,		_2.m_end)
				);
			}

			IC	static bool increment(self_type &it)
			{
				if (it.m_iterator != it.m_end)
					if (it.m_used)
						++it.m_iterator;
					else
						it.m_used	= true;

				if (it.m_iterator == it.m_end)
					return			(false);

				setup_value			(it.m_current,it.m_iterator);
				return				(true);
			}

			template <typename P>
			IC	static bool	try_convert(P &result, self_type &it, bool &found)
			{
				if (it.m_iterator == it.m_end)
					return		(false);

				found			= true;
				return			(_try_convert(result,it.m_iterator));
			}
		};

		template <typename _type>
		struct iterator_helper<_type,Loki::EmptyType>
		{
		public:
			typedef iterator_helper<_type,Loki::EmptyType>				self_type;
			typedef Loki::Typelist<self_type,Loki::NullType>			self_list;
			typedef typename core_container::type<_type>::result		result;
			typedef typename result::iterator							iterator;
			typedef typename result::value_type							value_type;
			typedef typename pointer_type<dest_type>::result			temp_value;
			typedef typename current_type<temp_value>::result			value;
			
		public:
			iterator	m_iterator;
			iterator	m_end;
			value		m_current;

		public:
			template <typename T>
			IC	bool		init	(const T *holder)
			{
				m_iterator	= holder->container((vertex_container<_type>*)0)->objects().begin();
				m_end		= holder->container((vertex_container<_type>*)0)->objects().end();
				if (m_iterator != m_end) {
					setup_value	(m_current,m_iterator);
					return		(true);
				}
				m_current	= 0;
				return		(false);
			}

			template <typename T>
			IC	bool		init	(const T *holder, bool end_iterator)
			{
				m_iterator	= m_end	= holder->container((vertex_container<_type>*)0)->objects().end();
				return		(false);
			}

			IC				iterator_helper	() :
				m_current	(0)
			{
			}

			IC	iterator_helper	&operator=(const iterator_helper &it)
			{
				*this = it;
			}

			template <template <typename _> class _predicate>
			IC	static bool compare(const self_type &_1, const self_type &_2)
			{
				return	(
					_predicate<iterator>()(_1.m_iterator,	_2.m_iterator) &&
					_predicate<iterator>()(_1.m_end,		_2.m_end)
				);
			}

			IC	static bool increment(self_type &it)
			{
				if (it.m_iterator != it.m_end)
					++it.m_iterator;

				if (it.m_iterator == it.m_end)
					return		(false);
				
				setup_value		(it.m_current,it.m_iterator);
				return			(true);
			}

			template <typename P>
			IC	static bool	try_convert(P &result, self_type &it, bool &found)
			{
				if (it.m_iterator == it.m_end)
					return		(false);

				found			= true;
				return			(_try_convert(result,it.m_iterator));
			}
		};
	};

	template <typename T, template <typename _> class _predicate>
	struct compare_ {
		typedef typename T::Head Head;
		typedef typename T::Tail Tail;

		IC	static bool _compare(const Head &_1, const Head &_2)
		{
			return	(
				compare_<Tail,_predicate>::_compare(_1,_2) &&
				Head::compare<_predicate>(_1,_2)
			);
		}
	};

	template <template <typename _> class _predicate>
	struct compare_<Loki::NullType,_predicate> {
		template <typename T>
		IC	static bool _compare(const T &_1, const T &_2)
		{
			return	(_predicate<bool>()(true,true));
		}
	};

	template <typename T>
	struct _increment {
		typedef typename T::Head Head;
		typedef typename T::Tail Tail;

		template <typename P>
		IC	static bool process(P &it)
		{
			if (!_increment<Tail>::process(it))
				return	(Head::increment(it));
			return		(true);
		}
	};

	template <>
	struct _increment<Loki::NullType> {
		template <typename P>
		IC	static bool process(P &it)
		{
			return		(false);
		}
	};

	template <typename T, typename T1, typename T2>
	struct try_converter {
		template <typename T>
		struct converter {
			typedef typename T::Head Head;
			typedef typename T::Tail Tail;

			IC	static bool process(T1 *&result, T2 &it, bool &found)
			{
				if (!converter<Tail>::process(result,it,found)) {
					if (!found)
						return	(Head::try_convert(result,it,found));
					else
						return	(false);
				}
				return			(true);
			}
		};

		template <>
		struct converter<Loki::NullType> {
			IC	static bool process(T1 *&result, T2 &it, bool &found)
			{
				result		= 0;
				return		(false);
			}
		};

		IC	static bool process(T1 *&result, T2 &it)
		{
			bool			found = false;
			return			(converter<T>::process(result,it,found));
		}
	};

	template <typename T>
	struct iterator : 
		public 
		hierarchy_generator<
			typename derived<T>::result,
			_iterator_helper<T>::iterator_helper
		>::result
	{
		typedef typename hierarchy_generator<
			typename derived<T>::result,
			_iterator_helper<T>::iterator_helper
		>::result inherited;
		typedef typename inherited::self_list self_list;
		typedef typename pointer_type<T>::result temp_value;
		typedef typename return_type<temp_value>::result value;

		bool			m_result;

		IC				iterator	()
		{
			NODEFAULT;
		}

		template <typename T>
		IC				iterator	(const T *holder)
		{
			m_result	= init(holder);
		}

		template <typename T>
		IC				iterator	(const T *holder, bool end_iterator)
		{
			init		(holder,end_iterator);
			m_result	= false;
		}

		IC	value		operator*	() const
		{
			return		(get_value<value>(m_current));
		}

		IC	temp_value	*operator->	() const
		{
			return		(&get_value<value>(m_current));
		}

		IC		bool	operator==	(const iterator &_1) const
		{
			return		(compare_<self_list,std::equal_to>::_compare(*this,_1));
		}

		IC		bool	operator!=	(const iterator &_1) const
		{
			return		(!(*this == _1));
		}

		IC		iterator operator++(int)
		{
			iterator	result = *this;
			++*this;
			return		(result);
		}

		IC		iterator &operator++()
		{
			VERIFY		(m_result);
			m_result	= _increment<self_list>::process(*this);
			return		(*this);
		}

		template <typename T>
		IC	bool try_convert(T *&result)
		{
			return		(try_converter<self_list,T,iterator>::process(result,*this));
		}

		template <typename T>
		IC	bool try_convert(T &result)
		{
			return		(try_convert(&result));
		}
	};

	template <typename T>
	IC	iterator<T> begin()
	{
		return	(iterator<T>(this));
	}

	template <typename T>
	IC	iterator<T> end()
	{
		return	(iterator<T>(this,true));
	}

	template <typename T>
	IC	const T* container(T*) const
	{
		return	(static_cast<const T*>(this));
	}

	virtual void	load(IReader &stream)
	{
		inherited::load(stream);
	}

	virtual void	save(IWriter &stream)
	{
		inherited::save(stream);
	}

	IC		bool	operator==(const _container &t) const
	{
		return		((inherited&)*this == t);
	}

	IC		void	clear	()
	{
		inherited::clear();
	}
};

typedef _container<Loki::TL::NoDuplicates<final_vertex_type_list>::Result> smart_container;