////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_container.h
//	Created 	: 22.10.2004
//  Modified 	: 22.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Smart container class
////////////////////////////////////////////////////////////////////////////

#pragma once

#define USE_BOOST_TRAITS

#ifdef USE_BOOST_TRAITS
#	include <boost/type_traits/is_base_and_derived.hpp>
#endif

#include "smart_container_types.h"
#include "object_broker.h"

namespace Loki {struct EmptyType{};};

template <typename T>
struct pointer_type {
	typedef T* result;
};

struct core_container {
	template <typename T>
	struct type {
		typedef typename pointer_type<T>::result	type_value;
		typedef std::vector<type_value>				result;
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

template <typename _vertex_type, typename _container_type = typename core_container::type<_vertex_type>::result>
class vertex_container {
public:
	typedef _vertex_type							_vertex_type;
	typedef _container_type							_container_type;
	typedef typename _container_type::value_type	value_type;

private:
	mutable _container_type	m_vertices;

public:
	IC				vertex_container	()
	{
	}

	virtual			~vertex_container	()
	{
		delete_data(m_vertices);
	}

	IC		void	add(const value_type &vertex)
	{
		core_container::add	(m_vertices,vertex);
	}

	IC	_container_type	&objects() const
	{
		return		(m_vertices);
	}

	virtual void	load(IReader &stream)
	{
		load_data	(m_vertices,stream);
	}

	virtual void	save(IWriter &stream)
	{
		save_data	(m_vertices,stream);
	}
};

template <typename _type, typename _base>
struct vertex_hierarchy_helper : public vertex_container<_type>, public _base
{
	using vertex_container<_type>::add;
	using _base::add;
	virtual ~vertex_hierarchy_helper(){};

	virtual void	load(IReader &stream)
	{
		vertex_container<_type>::load	(stream);
		_base::load						(stream);
	}

	virtual void	save(IWriter &stream)
	{
		vertex_container<_type>::save	(stream);
		_base::save						(stream);
	}
};

template <typename _type>
struct vertex_hierarchy_helper<_type,Loki::EmptyType> : public vertex_container<_type>, public IPureSerializeObject
{
	using vertex_container<_type>::add;
	virtual ~vertex_hierarchy_helper(){};

	virtual void	load(IReader &stream)
	{
		vertex_container<_type>::load	(stream);
	}

	virtual void	save(IWriter &stream)
	{
		vertex_container<_type>::save	(stream);
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
			IC				init	(const T *holder)
			{
				_base::init	(holder);
				m_iterator	= holder->container((vertex_container<_type>*)0)->objects().begin();
				m_end		= holder->container((vertex_container<_type>*)0)->objects().end();
				m_used		= false;
			}

			template <typename T>
			IC				init	(const T *holder, bool end_iterator)
			{
				_base::init	(holder,end_iterator);
				m_iterator	= m_end	= holder->container((vertex_container<_type>*)0)->objects().end();
				m_used		= true;
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
			IC				init	(const T *holder)
			{
				m_iterator	= holder->container((vertex_container<_type>*)0)->objects().begin();
				m_end		= holder->container((vertex_container<_type>*)0)->objects().end();
				setup_value	(m_current,m_iterator);
			}

			template <typename T>
			IC				init	(const T *holder, bool end_iterator)
			{
				m_iterator	= m_end	= holder->container((vertex_container<_type>*)0)->objects().end();
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
			init		(holder);
			m_result	= *this != iterator(holder,true);
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
};

typedef _container<final_vertex_type_list> container;