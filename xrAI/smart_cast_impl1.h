////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_cast_impl1.h
//	Created 	: 17.09.2004
//  Modified 	: 17.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Smart dynamic cast implementation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <_type_traits.h>
#include "object_type_traits.h"

namespace SmartDynamicCast {
	
	template <typename Base, typename Source, typename List>
	struct has_conversion {
		template <typename T>
		struct search_base {
			typedef typename T::Head Head;
			typedef typename T::Tail Tail;

			template <typename P>
			struct base_found {
				typedef typename search_base<Tail>::result result;
			};
			
			template <>
			struct base_found<Base> {
				typedef Head result;
			};

			typedef typename base_found<typename Head::Head>::result result;
		};
		
		template <>
		struct search_base<Loki::NullType> {
			typedef Loki::NullType result;
		};

		template <typename T>
		struct search_conversion {
			enum { value = object_type_traits::is_same<typename T::Head,Source>::value || search_conversion<typename T::Tail>::value};
		};

		template <>
		struct search_conversion<Loki::NullType> {
			enum { value = false};
		};

		enum { value = search_conversion<search_base<List>::result>::value};
	};

	template <typename Target, typename Source, typename List>
	struct CMatcher {

		template <typename T>
		struct CMatchHelper;

		template <typename HeadTail, typename T>
		struct CMatchHelper2 {
			template <typename P>
			struct CMatchHelper3 {
				typedef typename P::Head	Head;
				typedef typename P::Tail	Tail;
				typedef typename T::Head	PrevHead;

				template <bool>
				struct selector {
					typedef typename CMatchHelper3<Tail>::result result;
				};

				template <>
				struct selector<true> {
					template <bool>
					struct type_selector {
						typedef Loki::Typelist<typename PrevHead::Head,Loki::Typelist<Head,Loki::Typelist<Target,Loki::NullType> > > result;
					};

					template <>
					struct type_selector<true> {
						typedef Loki::Typelist<typename PrevHead::Head,Loki::Typelist<Target,Loki::NullType> > result;
					};

					typedef typename type_selector<object_type_traits::is_same<Head,typename PrevHead::Head>::value>::result result;
				};

				typedef typename selector<has_conversion<Head,Target,List>::value>::result result;
			};

			template <>
			struct CMatchHelper3<Loki::NullType> {
				typedef typename CMatchHelper<typename T::Tail>::result result;
			};

			typedef typename CMatchHelper3<HeadTail>::result result;
		};

		template <typename T>
		struct CMatchHelper {
			typedef typename T::Head		Head;
			typedef typename T::Tail		Tail;
			typedef typename Head::Tail		HeadTail;

			template <bool>
			struct selector {
				typedef typename CMatchHelper<Tail>::result result;
			};

			template <>
			struct selector<true> {
				typedef typename CMatchHelper2<Head,T>::result result;
			};

			typedef typename selector<object_type_traits::is_base_and_derived<typename Head::Head,Source>::value || object_type_traits::is_same<typename Head::Head,Source>::value>::result result;
		};

		template <>
		struct CMatchHelper<Loki::NullType> {
			typedef Loki::NullType result;
		};

		typedef typename CMatchHelper<List>::result result;
	};

	template <typename T, typename Target>
	struct CSmartCaster {
		typedef typename T::Head		Head;
		typedef typename T::Tail		Tail;
		typedef typename Tail::Head		NextHead;

		template <typename P>
		struct CHelper {
			IC	static Target* smart_cast(Head *p)
			{
				return		(CSmartCaster<Tail,Target>::smart_cast(SmartDynamicCast::smart_cast<NextHead>(p)));
			}
		};

		template <>
		struct CHelper<Loki::NullType> {
			IC	static Target* smart_cast(Head *p)
			{
				return		(SmartDynamicCast::smart_cast<Target>(p));
			}
		};

		IC	static Target* smart_cast(Head *p)
		{
			if (!p)
				return	(reinterpret_cast<Target*>(p));
			return		(CHelper<typename Tail::Tail>::smart_cast(p));
		}
	};

	template <typename T1, typename T2>
	struct CSmartMatcher {
		template <typename T3>
		IC	static T1* smart_cast(T2 *p)
		{
			return		(CSmartCaster<T3,T1>::smart_cast(static_cast<typename T3::Head*>(p)));
		}

		template <>
		IC	static T1* smart_cast<Loki::NullType>(T2 *p)
		{
			return		(dynamic_cast<T1*>(p));
		}
	};

	template <typename T1, typename T2>
	struct CHelper1 {
		template <bool base>
		IC	static T1* smart_cast(T2 *p)
		{
			return				(CSmartMatcher<T1,T2>::smart_cast<CMatcher<T1,T2,cast_type_list>::result>(p));
		}

		template <>
		IC	static T1* smart_cast<true>(T2 *p)
		{
			return				(static_cast<T1*>(p));
		}
	};

	template <typename T1, typename T2>
	IC	T1* smart_cast(T2 *p)
	{
		return					(CHelper1<T1,T2>::smart_cast<object_type_traits::is_base_and_derived<T1,T2>::value || object_type_traits::is_same<T1,T2>::value>(p));
	}

	template <typename T2>
	struct CHelper2 {
		template <typename T1>
		IC	static T1* smart_cast(T2* p)
		{
			STATIC_CHECK		(!object_type_traits::is_const<T2>::value || object_type_traits::is_const<T1>::value,Cannot_use_smart_cast_to_convert_const_to_non_const);
			typedef object_type_traits::remove_const<T1>::type _T1;
			typedef object_type_traits::remove_const<T2>::type _T2;
#ifdef DEBUG
			T1					*temp = SmartDynamicCast::smart_cast<_T1>(const_cast<_T2*>(p));
			VERIFY2				(temp == dynamic_cast<T1*>(p),"SmartCast result differs from the DynamicCast!");
			return				(temp);
#else
			return				(SmartDynamicCast::smart_cast<_T1>(const_cast<_T2*>(p)));
#endif
		}

		template <>
		IC	static void* smart_cast<void>(T2* p)
		{
			return				(dynamic_cast<void*>(p));
		}
	};
};

template <typename T1, typename T2>
IC	T1	smart_cast(T2* p)
{
#ifdef PURE_DYNAMIC_CAST_COMPATIBILITY_CHECK
	STATIC_CHECK				(object_type_traits::is_pointer<T1>::value,Invalid_target_type_for_Dynamic_Cast);
	STATIC_CHECK				(object_type_traits::is_void<object_type_traits::remove_pointer<T1>::type>::value || is_polymorphic<object_type_traits::remove_pointer<T1>::type>::result,Invalid_target_type_for_Dynamic_Cast);
	STATIC_CHECK				(is_polymorphic<T2>::result,Invalid_source_type_for_Dynamic_Cast);
#endif
	if (!p)
		return					(reinterpret_cast<T1>(p));
	return						(SmartDynamicCast::CHelper2<T2>::smart_cast<object_type_traits::remove_pointer<T1>::type>(p));
}

template <typename T1, typename T2>
IC	T1	smart_cast(T2& p)
{
#ifdef PURE_DYNAMIC_CAST_COMPATIBILITY_CHECK
	STATIC_CHECK				(object_type_traits::is_reference<T1>::value,Invalid_target_type_for_Dynamic_Cast);
	STATIC_CHECK				(is_polymorphic<object_type_traits::remove_reference<T1>::type>::result,Invalid_target_type_for_Dynamic_Cast);
	STATIC_CHECK				(is_polymorphic<T2>::result,Invalid_source_type_for_Dynamic_Cast);
#endif
	return						(*SmartDynamicCast::CHelper2<T2>::smart_cast<object_type_traits::remove_reference<T1>::type>(&p));
}

#ifdef XRGAME_EXPORTS
	template <> extern
	CGameObject* SmartDynamicCast::smart_cast<CGameObject,CObject>(CObject *p);
#endif
