////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_cast.h
//	Created 	: 17.09.2004
//  Modified 	: 17.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Smart dynamic cast
////////////////////////////////////////////////////////////////////////////

#pragma once

//#define PURE_DYNAMIC_CAST

#ifdef PURE_DYNAMIC_CAST
#	define smart_cast smart_cast
#else
#	include <_type_traits.h>
#	include "object_type_traits.h"


	class CObject;
	class CGameObject;
	class CEntity;
	class CEntityAlive;
	class CInventoryItem;
	class CInventoryOwner;
	class CActor;

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

		template <>
		CGameObject* smart_cast<CGameObject,CObject>(CObject *p);

		template <> extern
		CEntity* smart_cast<CEntity,CGameObject>(CGameObject *p);

		template <> extern
		CEntityAlive* smart_cast<CEntityAlive,CGameObject>(CGameObject *p);

		template <> extern
		CInventoryItem* smart_cast<CInventoryItem,CGameObject>(CGameObject *p);

		template <> extern
		CInventoryOwner* smart_cast<CInventoryOwner,CGameObject>(CGameObject *p);

		template <> extern
		CActor* smart_cast<CActor,CGameObject>(CGameObject *p);

		template <> extern
		CGameObject* smart_cast<CGameObject, CInventoryOwner>(CInventoryOwner *p);
	};

	template <typename T1, typename T2>
	IC	T1	smart_cast(T2* p)
	{
		if (!p)
			return					(0);
		STATIC_CHECK				(object_type_traits::is_pointer<T1>::value,Invalid_source_or_and_target_type_for_Dynamic_Cast);
		return						(SmartDynamicCast::CHelper2<T2>::smart_cast<object_type_traits::remove_pointer<T1>::type>(p));
	}

	template <typename T1, typename T2>
	IC	T1	smart_cast(T2& p)
	{
		STATIC_CHECK				(object_type_traits::is_reference<T1>::value,Invalid_source_or_and_target_type_for_Dynamic_Cast);
		return						(*SmartDynamicCast::CHelper2<T2>::smart_cast<object_type_traits::remove_reference<T1>::type>(&p));
	}


	template <> extern
	CGameObject* SmartDynamicCast::smart_cast<CGameObject,CObject>(CObject *p);

	template <> extern
	CEntity* SmartDynamicCast::smart_cast<CEntity,CGameObject>(CGameObject *p);

	template <> extern
	CEntityAlive* SmartDynamicCast::smart_cast<CEntityAlive,CGameObject>(CGameObject *p);

	template <> extern
	CInventoryItem* SmartDynamicCast::smart_cast<CInventoryItem,CGameObject>(CGameObject *p);

	template <> extern
	CInventoryOwner* SmartDynamicCast::smart_cast<CInventoryOwner,CGameObject>(CGameObject *p);

	template <> extern
	CActor* SmartDynamicCast::smart_cast<CActor,CGameObject>(CGameObject *p);

	template <> extern
	CGameObject* SmartDynamicCast::smart_cast<CGameObject, CInventoryOwner>(CInventoryOwner *p);

#endif