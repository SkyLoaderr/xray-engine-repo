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

#define DECLARE_SPECIALIZATION(B,A,C) \
	class A;\
	class B;\
	template <> extern\
	B* SmartDynamicCast::smart_cast<B,A>(A *p);\
	template <> extern\
	const B* SmartDynamicCast::smart_cast<const B,const A>(const A *p);

namespace SmartDynamicCast {
	
	template <typename T1, typename T2>
	struct CHelper1 {
		template <bool base>
		IC	static T1* smart_cast(T2 *p)
		{
			return				(dynamic_cast<T1*>(p));
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
			return				(SmartDynamicCast::smart_cast<T1>(p));
		}

		template <>
		IC	static void* smart_cast<void>(T2* p)
		{
			if (!p)
				return			((void*)0);
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
		return					(0);
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

class CObject;
class CGameObject;

template <> extern
CGameObject* SmartDynamicCast::smart_cast<CGameObject,CObject>(CObject *p);
