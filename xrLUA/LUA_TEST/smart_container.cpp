#include "stdafx.h"	

#pragma warning(push)
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#include "xrCore.h"
#include <mmsystem.h>
#pragma warning(pop)

#undef STATIC_CHECK
#include <typelist.h>
#include <hierarchygenerators.h>

#include <boost/type_traits/is_base_and_derived.hpp>

struct CPositionComponent {
	float  x, y, z; // position
};

struct CDiffuseComponent {
	DWORD  diffuse; //  diffuse vertex color
};

struct CFirstTCComponent {
	float  u, v;    //  1st texture coordinate set
};

struct CSecondTCComponent {
	float  u2, v2;      //  2nd texture coordinate set
};

struct CNormalComponent {
	float  nx, ny, nz;  // normal
};

struct VertexA : 
	public CPositionComponent,
	public CDiffuseComponent,
	public CFirstTCComponent
{
};

struct VertexB : 
	public CPositionComponent,
	public CNormalComponent,
	public CFirstTCComponent,
	public CSecondTCComponent
{
};

template <typename _vertex_type, template <typename _1> class _container_type = xr_vector>
class vertex_container {
public:
	typedef _vertex_type _vertex_type;
	typedef _container_type<_vertex_type> _container_type;

private:
	mutable _container_type	m_vertices;

public:
	IC				vertex_container	(){};
	
	virtual			~vertex_container	(){};
	
	IC		void	add(const _vertex_type &vertex)
	{
		m_vertices.push_back	(vertex);
	}

	IC	_container_type	&objects() const
	{
		return		(m_vertices);
	}
};

#define final_vertex_type_list Loki::NullType

#define vertex_type_add(a,b) \
	typedef Loki::Typelist<a,final_vertex_type_list> vertex_type_list_##a;\
	const vertex_container<a>* b = 0;

#define current_vertex_type_list(a) vertex_type_list_##a

vertex_type_add(VertexA,vertex_a)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(VertexA)

vertex_type_add(VertexB,vertex_b)
#undef final_vertex_type_list
#define final_vertex_type_list current_vertex_type_list(VertexB)

template <typename List>
struct container_hierarchy {
	template <typename T>
	struct helper {
		typedef typename T::Head Head;
		typedef typename T::Tail Tail;

		typedef Loki::Typelist<vertex_container<Head>,typename helper<Tail>::result> result;
	};

	template <>
	struct helper<Loki::NullType> {
		typedef Loki::NullType result;
	};

	typedef typename helper<List>::result result;
};

template <typename _type, typename _base>
struct vertex_hierarchy_helper : public _base, public vertex_container<_type>
{
	using vertex_container<_type>::add;
	using _base::add;
};

template <typename _type>
struct vertex_hierarchy_helper<_type,Loki::EmptyType> : public Loki::EmptyType, public vertex_container<_type>
{
	using vertex_container<_type>::add;
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

	template <typename _type, typename _base>
	struct iterator_helper : public _base
	{
		typedef typename xr_vector<_type>::iterator iterator;
		iterator		m_iterator;
		iterator		m_end;

		template <typename T>
		IC				iterator_helper	(const T *holder) :
			_base		(holder)
		{
			m_iterator	= holder->container((vertex_container<_type>*)0)->objects().begin();
			m_end		= holder->container((vertex_container<_type>*)0)->objects().end();
		}

		template <typename T>
		IC				iterator_helper	(const T *holder, bool end_iterator) :
			_base		(holder,end_iterator)
		{
			m_iterator	= m_end	= holder->container((vertex_container<_type>*)0)->objects().end();
		}

		IC				iterator_helper	()
		{
			NODEFAULT;
		}

		template <typename T, typename _predicate>
		IC	bool		compare(const T &_1, const T &_2) const
		{
			return		(
				_predicate(
					static_cast<const iterator_helper &>(_1).m_iterator,
					static_cast<const iterator_helper &>(_2).m_iterator,
				) &&
				_predicate(
					static_cast<const iterator_helper &>(_1).m_end,
					static_cast<const iterator_helper &>(_2).m_end,
				)
			);
		}
	};

	template <typename _type>
	struct iterator_helper<_type,Loki::EmptyType>
	{
		typedef typename xr_vector<_type>::iterator iterator;
		iterator		m_iterator;
		iterator		m_end;
		_type			*m_current;

		template <typename T>
		IC				iterator_helper	(const T *holder)
		{
			m_iterator	= holder->container((vertex_container<_type>*)0)->objects().begin();
			m_end		= holder->container((vertex_container<_type>*)0)->objects().end();
			m_current	= &*m_iterator;
		}

		template <typename T>
		IC				iterator_helper	(const T *holder, bool end_iterator)
		{
			m_iterator	= m_end	= holder->container((vertex_container<_type>*)0)->objects().end();
			m_current	= 0;
		}

		IC				iterator_helper	()
		{
			NODEFAULT;
		}

		template <typename T, typename _predicate>
		IC	bool		compare(const T &_1, const T &_2) const
		{
			return		(
				_predicate(
					static_cast<const iterator_helper &>(_1).m_iterator,
					static_cast<const iterator_helper &>(_2).m_iterator,
				) &&
				_predicate(
					static_cast<const iterator_helper &>(_1).m_end,
					static_cast<const iterator_helper &>(_2).m_end,
				) &&
				_predicate(
					static_cast<const iterator_helper &>(_1).m_current,
					static_cast<const iterator_helper &>(_2).m_current,
				)
			);
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

		template <typename _predicate>
		struct compare {
			typedef typename T::Head Head;
			typedef typename T::Tail Tail;

			template <typename P>
			struct helper {
				template <typename T>
				IC	bool compare(const T &_1, const T &_2)
				{
					return	(
						_predicate(
							static_cast<const iterator_helper<T>&>(_1),
							static_cast<const iterator_helper<T>&>(_2)
						) && 
						helper<Tail>::compare(_1,_2)
					);
				}
			};

			template <>
			struct helper<Loki::NullType> {
				template <typename T>
				IC	bool compare(const T &_1, const T &_2)
				{
					return	(_predicate(true,true));
				}
			};

			template <typename T>
			IC	bool compare(const T &_1, const T &_2)
			{
				return	(helper<iterator_list>::compare(_1,_2));
			}
		}
		
		IC				iterator	()
		{
			NODEFAULT;
		}

		template <typename T>
		IC				iterator	(const T *holder) :
			inherited	(holder)
		{
		}

		template <typename T>
		IC				iterator	(const T *holder, bool end_iterator) :
			inherited	(holder,end_iterator)
		{
		}

		IC		T		&operator*	() const
		{
			VERIFY		(m_current);
			return		(*m_current);
		}

		IC		T		*operator->	() const
		{
			VERIFY		(m_current);
			return		(m_current);
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

void test_smart_container()
{
	container		m_container;
	m_container.add	(VertexA());
	m_container.add	(VertexB());

//	m_container.container(vertex_a).begin<CDiffuseComponent>()
//
	container::iterator<CDiffuseComponent>	I = m_container.begin<CDiffuseComponent>();
	container::iterator<CDiffuseComponent>	E = m_container.end<CDiffuseComponent>();
	(*I).diffuse	+= 1;
	I->diffuse		+= 1;
	printf			("%s\n",I == E ? "TRUE" : "FALSE");
//	printf			("%s\n",I != E ? "TRUE" : "FALSE");
//	++I;
//	I++;
//	for ( ; I != E; ++I)
//		(*I).diffuse	+= 1.f;
}