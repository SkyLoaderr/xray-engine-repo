////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_cast.h
//	Created 	: 17.09.2004
//  Modified 	: 17.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Smart dynamic cast
////////////////////////////////////////////////////////////////////////////

#ifndef SMART_CAST_H
#define SMART_CAST_H

#define  TL_FAST_COMPILATION
#undef   STATIC_CHECK
#include <typelist.h>

//#define PURE_DYNAMIC_CAST
#define PURE_DYNAMIC_CAST_COMPATIBILITY_CHECK

#ifdef PURE_DYNAMIC_CAST
#	define smart_cast dynamic_cast
#else
#	ifndef DECLARE_SPECIALIZATION
#		include "smart_cast_impl0.h"
#	else
#		include "smart_cast_impl2.h"
#		define DO_NOT_DECLARE_TYPE_LIST
#	endif

#	ifdef XRGAME_EXPORTS

		DECLARE_SPECIALIZATION			(CKinematics,		IRender_Visual,		dcast_PKinematics);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CKinematics,		IRender_Visual)

		DECLARE_SPECIALIZATION			(CSkeletonRigid,	IRender_Visual,		dcast_PSkeletonRigid);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CSkeletonRigid,	IRender_Visual)

		DECLARE_SPECIALIZATION			(CSkeletonAnimated,	IRender_Visual,		dcast_PSkeletonAnimated);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CSkeletonAnimated,	IRender_Visual)

		DECLARE_SPECIALIZATION			(IParticleCustom,	IRender_Visual,		dcast_ParticleCustom);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(IParticleCustom,	IRender_Visual)

#		ifndef DO_NOT_DECLARE_TYPE_LIST
			class ENGINE_API ISpatial;
			namespace Feel { class ENGINE_API Sound; }
			typedef Feel::Sound Feel__Sound;
			template <> extern\
			Feel::Sound* SmartDynamicCast::smart_cast<Feel::Sound,ISpatial>(ISpatial *p);\
			add_to_cast_list(Feel__Sound,ISpatial);
#			undef cast_type_list
#			define cast_type_list save_cast_list	(Feel__Sound,		ISpatial)
#		endif

		DECLARE_SPECIALIZATION			(IRenderable,		ISpatial,			dcast_Renderable);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(IRenderable,		ISpatial)

		DECLARE_SPECIALIZATION			(IRender_Light,		ISpatial,			dcast_Light);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(IRender_Light,		ISpatial)

		DECLARE_SPECIALIZATION			(CObject,			ISpatial,			dcast_CObject);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CObject,			ISpatial)

#		ifndef DO_NOT_DECLARE_TYPE_LIST
			class CObject;
			class CGameObject;
			add_to_cast_list						(CGameObject,		CObject);
#			undef cast_type_list
#			define cast_type_list save_cast_list	(CGameObject,		CObject)
#		endif

		DECLARE_SPECIALIZATION	(CEntity,			CGameObject,		cast_entity);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CEntity,			CGameObject)

		DECLARE_SPECIALIZATION	(CEntityAlive,		CGameObject,		cast_entity_alive);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CEntityAlive,		CGameObject)

		DECLARE_SPECIALIZATION	(CInventoryItem,	CGameObject,		cast_inventory_item);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CInventoryItem,	CGameObject)

		DECLARE_SPECIALIZATION	(CInventoryOwner,	CGameObject,		cast_inventory_owner);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CInventoryOwner,	CGameObject)

		DECLARE_SPECIALIZATION	(CActor,			CGameObject,		cast_actor);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CActor,			CGameObject)

		DECLARE_SPECIALIZATION	(CGameObject,		CInventoryOwner,	cast_game_object);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CGameObject,		CInventoryOwner)

		DECLARE_SPECIALIZATION	(CAI_Rat,			CCustomMonster,		dcast_Rat);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CAI_Rat,			CCustomMonster)

#	endif
	
#	ifndef DO_NOT_DECLARE_TYPE_LIST
#		include "smart_cast_impl1.h"
#	endif
#endif

#endif //SMART_CAST_H