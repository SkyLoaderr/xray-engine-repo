////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_container.h
//	Created 	: 22.10.2004
//  Modified 	: 22.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Smart container class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <hierarchygenerators.h>
#include <boost/type_traits/is_base_and_derived.hpp>
#include "smart_container_types.h"
#include "object_broker.h"

#define core_container xr_vector

template <typename _vertex_type, template <typename _1> class _container_type = core_container>
class vertex_container {
public:
	typedef _vertex_type _vertex_type;
	typedef _container_type<_vertex_type*> _container_type;

private:
	mutable _container_type	m_vertices;

public:
	IC				vertex_container	(){};
	
	virtual			~vertex_container	(){ delete_data(m_vertices);};
	
	IC		void	add(_vertex_type *vertex)
	{
		m_vertices.push_back	(vertex);
	}

	IC	_container_type	&objects() const
	{
		return		(m_vertices);
	}
};

template <typename _type, typename _base>
struct vertex_hierarchy_helper : public vertex_container<_type>, public _base
{
	using vertex_container<_type>::add;
	using _base::add;
	virtual ~vertex_hierarchy_helper(){};
};

template <typename _type>
struct vertex_hierarchy_helper<_type,Loki::EmptyType> : public vertex_container<_type>
{
	using vertex_container<_type>::add;
	virtual ~vertex_hierarchy_helper(){};
};

template <typename type_list>
struct _container : public 
	Loki::GenLinearHierarchy<
		type_list,
		vertex_hierarchy_helper
	>::LinBase
{
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

			typedef typename selector<boost::is_base_and_derived<Type,Head>::value>::result result;
		};

		template <>
		struct helper<Loki::NullType> {
			typedef Loki::NullType result;
		};

		typedef typename helper<type_list>::result result;
	};

	virtual ~_container(){}

	template <typename _type, typename _base>
	struct iterator_helper : public _base
	{
		typedef iterator_helper<_type,_base> self_type;
		typedef Loki::Typelist<self_type,typename _base::self_list> self_list;
		typedef typename core_container<_type*>::iterator iterator;
		typedef _base inherited;

		iterator		m_iterator;
		iterator		m_end;
		bool			m_used;

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
					it.m_used = true;

			if (it.m_iterator != it.m_end)
				return	(true);
			return		(false);
		}
	};

	template <typename _type>
	struct iterator_helper<_type,Loki::EmptyType>
	{
		typedef iterator_helper<_type,Loki::EmptyType> self_type;
		typedef Loki::Typelist<self_type,Loki::NullType> self_list;
		typedef typename core_container<_type*>::iterator iterator;
		typedef typename core_container<_type*>::iterator self_iterator;
		typedef Loki::EmptyType inherited;
		
		iterator		m_iterator;
		iterator		m_end;
		_type			**m_current;

		template <typename T>
		IC				init	(const T *holder)
		{
			m_iterator	= holder->container((vertex_container<_type>*)0)->objects().begin();
			m_end		= holder->container((vertex_container<_type>*)0)->objects().end();
			m_current	= &*m_iterator;
		}

		template <typename T>
		IC				init	(const T *holder, bool end_iterator)
		{
			m_iterator	= m_end	= holder->container((vertex_container<_type>*)0)->objects().end();
			m_current	= 0;
		}

		IC				iterator_helper	()
		{
		}

		template <template <typename _> class _predicate>
		IC	static bool compare(const self_type &_1, const self_type &_2)
		{
			return	(
				_predicate<self_iterator>()(_1.m_iterator,	_2.m_iterator) &&
				_predicate<self_iterator>()(_1.m_end,		_2.m_end)
			);
		}

		IC	static bool increment(self_type &it)
		{
			if (it.m_iterator != it.m_end)
				++it.m_iterator;

			if (it.m_iterator != it.m_end)
				return	(true);
			return		(false);
		}
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
		public Loki::GenLinearHierarchy<
			typename derived<T>::result,
			iterator_helper
		>::LinBase
	{
		typedef typename derived<T>::result iterator_list;
		typedef typename Loki::GenLinearHierarchy<
			iterator_list,
			iterator_helper
		>::LinBase		inherited;
		typedef typename inherited::self_list self_list;

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

		IC		T		*operator*	() const
		{
			VERIFY		(m_current);
			return		(*m_current);
		}

		IC		T		**operator->	() const
		{
			VERIFY		(m_current);
			return		(m_current);
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
};

typedef _container<final_vertex_type_list> container;